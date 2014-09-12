/*
 * Neighborhood.cpp
 *
 *  Created on: Sep 10, 2014
 *      Author: goldman
 */

#include "Neighborhood.h"

using namespace std;

Neighborhood::Neighborhood(shared_ptr<GrayBox> evaluator, size_t radius) {
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
  vector<vector<size_t>> found;
  for (const auto& kv: graph) {
    closed.insert(kv.first);
    recurse(graph, kv.first, closed, prev, prevopen, radius);
  }

  // Set up mappings between subfunctions and moves
  unordered_map<size_t, vector<size_t>> bit_to_sub;
  for (size_t sub=0; sub < subfunctions.size(); sub++) {
    for (const auto& bit: subfunctions[sub]) {
      bit_to_sub[bit].push_back(sub);
    }
  }
  sub_to_move.resize(subfunctions.size());
  move_to_sub.resize(moves_.size());
  for (size_t m=0; m < moves_.size(); m++) {
    for (const auto& bit: moves_[m]) {
      for (const auto& sub: bit_to_sub[bit]) {
        move_to_sub[m].insert(sub);
        sub_to_move[sub].insert(m);
      }
    }
  }

  // Set up tools for random set tests
  options.resize(moves_.size());
  indexing.resize(moves_.size());
  iota(options.begin(), options.end(), 0);
  iota(indexing.begin(), indexing.end(), 0);
  useful = options.size();
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

void Neighborhood::sanity_check() {
  for (size_t i=0; i < options.size(); i++) {
    if (options[indexing[i]] != i) {
      std::cout << "Error" << endl;
    }
  }
}
