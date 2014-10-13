/*
 * Neighborhood.cpp
 *
 *  Created on: Sep 10, 2014
 *      Author: goldman
 */

#include "Neighborhood.h"

using namespace std;

Neighborhood::Neighborhood(shared_ptr<GrayBox> evaluator_, size_t radius) {
  evaluator = evaluator_;
  // Construct neighborhood
  unordered_map<size_t, unordered_set<size_t>> graph;
  const auto& subfunctions = evaluator->epistasis();
  for (auto subfunction: subfunctions) {
    for (auto x: subfunction) {
      for (auto y: subfunction) {
        if (x != y) {
          graph[x].insert(y);
        }
      }
    }
  }
  unordered_set<size_t> closed, prevopen;
  vector<size_t> prev;
  //vector<vector<size_t>> found;
  for (const auto& kv: graph) {
    closed.insert(kv.first);
    recurse(graph, kv.first, closed, prev, prevopen, radius);
  }

  // Set up mappings between subfunctions and moves
  for (size_t sub=0; sub < subfunctions.size(); sub++) {
    for (const auto& bit: subfunctions[sub]) {
      bit_to_sub[bit].push_back(sub);
    }
  }
  sub_to_move.resize(subfunctions.size());
  move_to_sub.resize(moves_.size());
  for (size_t m=0; m < moves_.size(); m++) {
    if (moves_[m].size() == 1) {
      single_bit_moves[moves_[m][0]] = m;
    }
    for (const auto& bit: moves_[m]) {
      for (const auto& sub: bit_to_sub[bit]) {
        move_to_sub[m].insert(sub);
        sub_to_move[sub].insert(m);
      }
    }
  }
  cout << "Single Bit Moves: " << single_bit_moves.size() << endl;
  // Set up tools for random set tests
  options.resize(moves_.size());
  indexing.resize(moves_.size());
  iota(options.begin(), options.end(), 0);
  iota(indexing.begin(), indexing.end(), 0);
  useful = options.size();
  delta.resize(options.size(), 0);
  fitness=0;
}

float Neighborhood::attach(vector<bool>* solution_) {
  solution = solution_;
  fitness=0;
  delta.assign(delta.size(), 0);
  for(size_t sub=0; sub < evaluator->epistasis().size(); sub++) {
    auto score = evaluator->evaluate(sub, *solution);
    fitness += score;
    for (auto move: sub_to_move[sub]) {
      delta[move] -= score;
      flip(move);
      delta[move] += evaluator->evaluate(sub, *solution);
      flip(move);
    }
  }
  useful = options.size();
  return fitness;
}

float Neighborhood::optimize(Random & rand) {
  size_t loops=0, improvements=0;
  while (useful) {
    loops ++;
    auto move = rand_useful(rand);
    //cout << "Testing: " << index << " fit: " << new_fitness << endl;
    if (0 < delta[move]) {
      improvements++;
      fitness = make_move(move); // Put in move
    }
    freeze(move);
  }
  /*
  cout << "Neighbors: " << moves_.size()
       << " Loops: " << loops
       << " Loop/Neighbor " << static_cast<float>(loops) / moves_.size()
       << " Improving moves: " << improvements
       << " Fitness " << fitness << " TAG " << endl;
  //*/
  return fitness;
}

float Neighborhood::make_bad(Random & rand) {
  size_t loops=0, improvements=0;
  while (useful) {
    loops ++;
    auto move = rand_useful(rand);
    //cout << "Testing: " << index << " fit: " << new_fitness << endl;
    if (0 > delta[move]) {
      improvements++;
      fitness = make_move(move); // Put in move
    }
    freeze(move);
  }
  return fitness;
}

void Neighborhood::set_check_point() {
  saved.clear();
  flipped.clear();
  check_point_fitness = fitness;
  useful=0;
  //temp = delta;
}

float Neighborhood::make_move(size_t move) {
  fitness += delta[move];
  for (auto sub: move_to_sub[move]) {
    auto pre_move = evaluator->evaluate(sub, *solution);
    flip(move); // Put in move
    auto just_move = evaluator->evaluate(sub, *solution);
    flip(move); // Take out move
    for (auto next: sub_to_move[sub]) {
      flip(next); // Put in next
      auto just_next = evaluator->evaluate(sub, *solution);
      flip(move); // Put in move
      auto move_next = evaluator->evaluate(sub, *solution);
      flip(move); // Take out move
      flip(next); // Take out next
      if (saved.count(next) == 0) {
        saved[next] = delta[next];
      }
      delta[next] += (pre_move - just_next + move_next - just_move);
      unfreeze(next);
    }
  }
  for (const auto& bit: moves_[move]) {
    if (flipped.count(bit)) {
      //cout << "Unflipping" << bit << endl;
      flipped.erase(bit);
    } else {
      //cout << "Flipping" << bit << endl;
      flipped.insert(bit);
    }
  }
  flip(move); // Put in move
  return fitness;
}

float Neighborhood::modify(size_t bit) {
  return make_move(single_bit_moves[bit]);
}

float Neighborhood::revert() {
  for (const auto& changed: saved) {
    delta[changed.first] = changed.second;
  }
  for (const auto& bit: flipped) {
    (*solution)[bit] = not (*solution)[bit];
  }
  fitness = check_point_fitness;
  /*
  if (temp != delta) {
    throw "SHIT";
  }
  */
  set_check_point();
  return fitness;
}

void Neighborhood::freeze(size_t index) {
  useful--;
  size_t moving = options[useful];
  // Swap where "moving" and "index" show up in options
  swap(options[useful], options[indexing[index]]);
  // Swap the pointers for "moving" and "index"
  swap(indexing[moving], indexing[index]);
}

void Neighborhood::unfreeze(size_t index) {
  if (indexing[index] >= useful) {
    size_t moving = options[useful];
    // Swap where "moving" and "index" show up in options
    swap(options[useful], options[indexing[index]]);
    // Swap the pointers for "moving" and "index"
    swap(indexing[moving], indexing[index]);
    useful++;
  }
}

size_t Neighborhood::rand_useful(Random& rand) {
  auto choose = uniform_int_distribution<size_t>(0, useful-1)(rand);
  return options[choose];
}

void Neighborhood::recurse(const unordered_map<size_t, unordered_set<size_t>>& graph,
                               size_t v, unordered_set<size_t> & closed, vector<size_t> & prev,
                               unordered_set<size_t> & prevopen, size_t radius) {
  vector<size_t> inset(prev);
  inset.push_back(v);
  sort(inset.begin(), inset.end());
  moves_.push_back(inset);
  if (inset.size() >= radius) {
    return;
  }
  vector<size_t> closed_here;
  unordered_set<size_t> openset(prevopen);
  const auto & adjacent = graph.at(v);
  openset.insert(adjacent.begin(), adjacent.end());
  for (const auto& working: openset) {
    if (closed.count(working) == 0) {
      closed_here.push_back(working);
      closed.insert(working);
      recurse(graph, working, closed, inset, openset, radius);
    }
  }
  for (const auto& working: closed_here) {
    closed.erase(working);
  }
}

void Neighborhood::flip(size_t index, vector<bool>& solution) {
  for (const auto& bit: moves_[index]) {
    solution[bit] = not solution[bit];
  }
}

void Neighborhood::flip(size_t index) {
  for (const auto& bit: moves_[index]) {
    (*solution)[bit] = not (*solution)[bit];
  }
}


void Neighborhood::sanity_check() {
  for (size_t i=0; i < options.size(); i++) {
    if (options[indexing[i]] != i) {
      std::cout << "Error" << endl;
    }
  }
}
