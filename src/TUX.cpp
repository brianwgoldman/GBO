/*
 * SAC.cpp
 *
 *  Created on: Oct 15, 2014
 *      Author: goldman
 */

#include "TUX.h"

TUX::TUX(Random& _rand, Configuration& _config, ImprovementHarness& _harness)
    : Optimizer(_rand, _config, _harness),
      coin(0.5) {
  offspring.resize(length);
  best_offspring.resize(length);
}

int TUX::iterate() {
  rand_vector(rand, solution);
  harness.attach(&solution);
  auto fitness = harness.optimize(rand);
  auto best_offspring = solution;
  for (size_t level = 0; level < solutions.size(); level++) {
    if (empty[level]) {
      // more efficient than assignment
      solutions[level].swap(solution);
      fitnesses[level] = fitness;
      empty[level] = false;
      return fitness;
    } else {
      auto best_fitness = fitness - 1;
      for (int repeat = 0; repeat < (1 << level); repeat++) {
        vector<bool> offspring(solution);
        for (size_t gene = 0; gene < offspring.size(); gene++) {
          // cross with other parent
          if (coin(rand)) {
            offspring[gene] = solutions[level][gene];
          } else {
            offspring[gene] = solution[gene];
          }
        }
        harness.attach(&offspring);
        int new_fitness = harness.optimize(rand);
        if (best_fitness < new_fitness) {
          best_offspring.swap(offspring);
          best_fitness = new_fitness;
        }
        // TODO Decide if this method is worth the complexity
        /*
        if (fitness < fitnesses[level]) {
          if (fitness < new_fitness) {
            // more efficient than assignment
            swap(solution, offspring);
            fitness = new_fitness;
          }
        } else {
          if (fitnesses[level] < new_fitness) {
            solutions[level].swap(offspring);
            fitnesses[level] = new_fitness;
          }
        }
        if (solutions[level] == solution) {
          break;
        }
        //*/
      }
      if (fitness < best_fitness) {
        fitness = best_fitness;
        solution.swap(best_offspring);
      }
      // stored is best
      if (fitness <= fitnesses[level]) {
        fitness = fitnesses[level];
        solution.swap(solutions[level]);
      }
      // current is best
      empty[level] = true;
    }
  }
  empty.push_back(false);
  fitnesses.push_back(fitness);
  solutions.push_back(solution);
  return fitness;
}
