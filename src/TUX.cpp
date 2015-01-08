/*
 * SAC.cpp
 *
 *  Created on: Oct 15, 2014
 *      Author: goldman
 */

#include "TUX.h"

TUX::TUX(Random& _rand, Configuration& _config, ImprovementHarness& _harness)
    : Optimizer(_rand, _config, _harness),
      rbit(0, 1) {
  offspring.resize(length);
  best_offspring.resize(length);
}

int TUX::iterate() {
  rand_vector(rand, solution);
  harness.attach(&solution);
  auto fitness = harness.optimize(rand);
  for (size_t level = 0; level < tournament.size(); level++) {
    if (empty[level]) {
      // more efficient than assignment
      tournament[level].swap(solution);
      fitnesses[level] = fitness;
      empty[level] = false;
      return fitness;
    } else {
      auto best_fitness = fitness - 1;
      for (int repeat = 0; repeat < (2 << level); repeat++) {
        for (size_t gene = 0; gene < offspring.size(); gene++) {
          // if parents are equal, copy from parent
          if (tournament[level][gene] == solution[gene]) {
            offspring[gene] = solution[gene];
          } else {
            // if unequal, offspring should be equally likely true and false.
            offspring[gene] = rbit(rand);
          }
        }
        harness.attach(&offspring);
        int new_fitness = harness.optimize(rand);
        if (best_fitness < new_fitness) {
          best_offspring.swap(offspring);
          best_fitness = new_fitness;
        }
      }
      if (fitness < best_fitness) {
        fitness = best_fitness;
        solution.swap(best_offspring);
      }
      // stored is best
      if (fitness < fitnesses[level]) {
        fitness = fitnesses[level];
        solution.swap(tournament[level]);
      }
      // current is best
      empty[level] = true;
    }
  }
  empty.push_back(false);
  fitnesses.push_back(fitness);
  tournament.push_back(solution);
  return fitness;
}
