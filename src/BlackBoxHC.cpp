/*
 * BlackBoxHC.cpp
 *
 *  Created on: Oct 21, 2014
 *      Author: goldman
 */

#include "BlackBoxHC.h"

BlackBoxHC::BlackBoxHC(Random& _rand, Configuration& _config,
                       ImprovementHarness& _harness)
    : Optimizer(_rand, _config, _harness) {
  options.resize(length);
  iota(options.begin(), options.end(), 0);
}

int BlackBoxHC::iterate() {
  rand_vector(rand, solution);
  int fitness = harness.evaluate(solution);
  int new_fitness;
  // keep track of locations already tried since last improvement
  std::unordered_set<int> tried;

  // Keep looping until there is no single bit flip improvement
  do {
    // Test the bits in a random order
    std::shuffle(options.begin(), options.end(), rand);
    for (const auto& index : options) {
      // If this location has already been tried, skip to the next one
      if (tried.count(index) != 0) {
        continue;
      }

      // flip and evaluate the modification
      solution[index] = not solution[index];
      new_fitness = harness.evaluate(solution);
      if (fitness < new_fitness) {
        // Keep change, update variables
        fitness = new_fitness;
        tried.clear();
      } else {
        // Revert the change
        solution[index] = not solution[index];
      }
      tried.insert(index);
    }
  } while (tried.size() < length);
  return fitness;
}

