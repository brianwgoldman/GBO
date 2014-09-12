// Brian Goldman

// Implementation of the various local search algorithms used in
// informal testing.

#include "HillClimb.h"
#include <algorithm>

using namespace std;

// Template specialization to allow Configuration to return
// a hill climber function pointer given a string key
template<>
hill_climb::pointer Configuration::get(const string key) {
  return hill_climb::lookup[get<string>(key)];
}

// Iteratively tests bits in a random order, accepting improvements as they
// are found.  Tracks bits that have been tested since the last modification
// to prevent waste.
float hill_climb::first_improvement(Random & rand, vector<bool> & solution, shared_ptr<GrayBox> evaluator, Neighborhood& neighborhood) {
  // Set up data structure for random bit selection
  vector<int> options(solution.size());
  iota(options.begin(), options.end(), 0);
  float new_fitness, fitness;
  bool improvement;
  fitness = 0;
  for(size_t i=0; i < evaluator->epistasis().size(); i++) {
    fitness += evaluator->evaluate(i, solution);
  }

  // keep track of locations already tried since last improvement
  std::unordered_set<int> tried;

  // Keep looping until there is no single bit flip improvement
  do {
    improvement = false;
    // Test the bits in a random order
    std::shuffle(options.begin(), options.end(), rand);
    for (const auto& index : options) {
      // If this location has already been tried, skip to the next one
      if (tried.count(index) != 0) {
        continue;
      }

      // flip and evaluate the modification
      solution[index] = not solution[index];
      new_fitness = 0;
      for (size_t i=0; i < evaluator->epistasis().size(); i++) {
        new_fitness += evaluator->evaluate(i, solution);
      }
      if (fitness < new_fitness) {
        // Keep change, update variables
        fitness = new_fitness;
        improvement = true;
        tried.clear();
      } else {
        // Revert the change
        solution[index] = not solution[index];
      }
      tried.insert(index);
    }
  } while (improvement);
  return fitness;
}

float hill_climb::first_alternative(Random & rand, vector<bool> & solution, shared_ptr<GrayBox> evaluator, Neighborhood& neighborhood) {
  // Set up data structure for random bit selection
  vector<int> options(solution.size(), 0), indexing(solution.size(), 0);
  iota(options.begin(), options.end(), 0);
  iota(indexing.begin(), indexing.end(), 0);
  size_t useful = options.size();
  float new_fitness, fitness;

  fitness = 0;
  for(size_t i=0; i < evaluator->epistasis().size(); i++) {
    fitness += evaluator->evaluate(i, solution);
  }
  // Linkage map
  unordered_map<int, vector<int>> bit_to_sub, sub_to_bit;
  for (size_t sub=0; sub < evaluator->epistasis().size(); sub ++) {
    // cout << "Sub: " << sub << endl;
    for (auto index: evaluator->epistasis()[sub]) {
      sub_to_bit[sub].push_back(index);
      bit_to_sub[index].push_back(sub);
    }
  }
  //cout << "-----------Starting-------------" << endl;
  size_t total = 0, loops=0;
  while (useful) {
    auto choose = uniform_int_distribution<size_t>(0, useful-1)(rand);
    auto index = options[choose];
    new_fitness = 0;
    total += bit_to_sub[index].size();
    loops ++;
    for (auto sub: bit_to_sub[index]) {
      new_fitness -= evaluator->evaluate(sub, solution);
    }
    solution[index] = not solution[index];
    for (auto sub: bit_to_sub[index]) {
      new_fitness += evaluator->evaluate(sub, solution);
    }

    //cout << "Testing: " << index << " fit: " << new_fitness << endl;
    if (0 < new_fitness) {
      // Keep change, update variables
      //cout << "Changed " << index << endl;
      fitness += new_fitness;
      for (auto problem: bit_to_sub[index]) {
        for (auto bit: sub_to_bit[problem]) {
          // if the bit isn't marked as useful
          if (useful <= indexing[bit]) {
            //cout << "\t" << bit << endl;
            auto moving = options[useful];
            swap(indexing[bit], indexing[moving]);
            swap(options[indexing[moving]], options[useful]);
            useful++;
          }
        }
      }
    } else {
      // Revert the change
      solution[index] = not solution[index];
    }
    useful--;
    swap(indexing[options[choose]], indexing[options[useful]]);
    swap(options[choose], options[useful]);
  }
  for (size_t i=0; i < options.size(); i++) {
    if (options[indexing[i]] != i) {
      std::cout << "Error" << endl;
    }
  }
  // cout << total / static_cast<float>(loops) << " " << loops << endl;
  return fitness;
}


float hill_climb::first_memory(Random & rand, vector<bool> & solution, shared_ptr<GrayBox> evaluator, Neighborhood& neighborhood) {
  // Set up data structure for random bit selection
  vector<int> options(solution.size(), 0), indexing(solution.size(), 0);
  iota(options.begin(), options.end(), 0);
  iota(indexing.begin(), indexing.end(), 0);
  size_t useful = options.size();
  float fitness = 0;
  vector<float> delta(solution.size(), 0);
  for(size_t i=0; i < evaluator->epistasis().size(); i++) {
    auto score = evaluator->evaluate(i, solution);
    fitness += score;
    for (auto bit: evaluator->epistasis()[i]) {
      delta[bit] -= score;
      solution[bit] = not solution[bit];
      delta[bit] += evaluator->evaluate(i, solution);
      solution[bit] = not solution[bit];
    }
  }
  // Linkage map
  unordered_map<int, vector<int>> bit_to_sub, sub_to_bit;
  for (size_t sub=0; sub < evaluator->epistasis().size(); sub ++) {
    // cout << "Sub: " << sub << endl;
    for (auto index: evaluator->epistasis()[sub]) {
      sub_to_bit[sub].push_back(index);
      bit_to_sub[index].push_back(sub);
    }
  }
  //cout << "-----------Starting-------------" << endl;
  size_t total = 0, loops=0;
  while (useful) {
    auto choose = uniform_int_distribution<size_t>(0, useful-1)(rand);
    auto index = options[choose];
    total += bit_to_sub[index].size();
    loops ++;
    //cout << "Testing: " << index << " fit: " << new_fitness << endl;
    if (0 < delta[index]) {
      solution[index] = not solution[index];
      // Keep change, update variables
      //cout << "Changed " << index << endl;
      fitness += delta[index];
      for (auto problem: bit_to_sub[index]) {
        solution[index] = not solution[index];
        auto pre_index = evaluator->evaluate(problem, solution);
        solution[index] = not solution[index];
        auto post_index = evaluator->evaluate(problem, solution);
        for (auto bit: sub_to_bit[problem]) {
          solution[bit] = not solution[bit];
          auto both_flip = evaluator->evaluate(problem, solution);
          solution[index] = not solution[index];
          auto bit_flip = evaluator->evaluate(problem, solution);
          solution[index] = not solution[index];
          solution[bit] = not solution[bit];
          delta[bit] += (pre_index - bit_flip + both_flip - post_index);
          // if the bit isn't marked as useful
          if (useful <= indexing[bit]) {
            //cout << "\t" << bit << endl;
            auto moving = options[useful];
            swap(indexing[bit], indexing[moving]);
            swap(options[indexing[moving]], options[useful]);
            useful++;
          }
        }
      }
    }
    useful--;
    swap(indexing[options[choose]], indexing[options[useful]]);
    swap(options[choose], options[useful]);
  }
  /*
  for (size_t i=0; i < options.size(); i++) {
    if (options[indexing[i]] != i) {
      std::cout << "Error" << endl;
    }
  }
  */
  // cout << total / static_cast<float>(loops) << " " << loops << endl;
  return fitness;
}

float hill_climb::neighbor_memory(Random & rand, vector<bool> & solution, shared_ptr<GrayBox> evaluator, Neighborhood& neighborhood) {
  neighborhood.reset();
  float fitness = 0;
  vector<float> delta(neighborhood.moves().size(), 0);
  for(size_t sub=0; sub < evaluator->epistasis().size(); sub++) {
    auto score = evaluator->evaluate(sub, solution);
    fitness += score;
    for (auto move: neighborhood.effected_moves(sub)) {
      delta[move] -= score;
      neighborhood.flip(move, solution);
      delta[move] += evaluator->evaluate(sub, solution);
      neighborhood.flip(move, solution);
    }
  }

  //cout << "-----------Starting-------------" << endl;
  size_t loops=0, improvements=0;
  while (neighborhood.untested()) {
    loops ++;
    auto move = neighborhood.rand_useful(rand);
    //cout << "Testing: " << index << " fit: " << new_fitness << endl;
    if (0 < delta[move]) {
      improvements++;
      // Keep change, update variables
      //cout << "Changed " << index << endl;
      fitness += delta[move];
      for (auto sub: neighborhood.effected_subfunctions(move)) {
        auto pre_move = evaluator->evaluate(sub, solution);
        neighborhood.flip(move, solution); // Put in move
        auto just_move = evaluator->evaluate(sub, solution);
        neighborhood.flip(move, solution); // Take out move
        for (auto next: neighborhood.effected_moves(sub)) {
          neighborhood.flip(next, solution); // Put in next
          auto just_next = evaluator->evaluate(sub, solution);
          neighborhood.flip(move, solution); // Put in move
          auto move_next = evaluator->evaluate(sub, solution);
          neighborhood.flip(move, solution); // Take out move
          neighborhood.flip(next, solution); // Take out next
          delta[next] += (pre_move - just_next + move_next - just_move);
          neighborhood.unfreeze(next);
        }
      }
      neighborhood.flip(move, solution); // Put in move
    }
    neighborhood.freeze(move);
  }
  cout << "Neighbors: " << neighborhood.moves().size()
       << " Loops: " << loops
       << " Loop/Neighbor " << static_cast<float>(loops) / neighborhood.moves().size()
       << " Improving moves: " << improvements
       << " Fitness " << fitness << endl;
  return fitness;
}
