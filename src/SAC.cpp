/*
 * SAC.cpp
 *
 *  Created on: Oct 15, 2014
 *      Author: goldman
 */

#include "SAC.h"

SAC::SAC(Random& _rand, Configuration& _config, ImprovementHarness& _harness)
    : Optimizer(_rand, _config, _harness), coin(0.5) {
}
#include <iostream>
using std::cout; using std::endl;

int SAC::iterate() {
  //cout << "Prestate" << endl;
  /*
  for (size_t level=0; level < solutions.size(); level++) {
    cout << level << " " << (empty[level]?-1: fitnesses[level]) << " | ";
  }
  cout << endl;
  */
  auto solution = rand_vector(rand, length);
  harness.attach(&solution);
  auto fitness = harness.optimize(rand);
  //cout << "Rand: " << fitness << endl;
  for (size_t level=0; level < solutions.size(); level++) {
    if (empty[level]) {
      // more efficient than assignment
      swap(solutions[level], solution);
      fitnesses[level] = fitness;
      empty[level] = false;
      return fitness;
    } else {
      auto best_offspring = solution;
      auto best_fitness = fitness;
      for (int repeat=0; repeat < (1<<level); repeat++) {
      //for (int repeat=0; repeat < level+1; repeat++) {
        vector<bool> offspring(solution);
        for (size_t gene=0; gene < offspring.size(); gene++) {
          // cross with other parent
          if (coin(rand)) {
            offspring[gene] = solutions[level][gene];
          }
        }
        harness.attach(&offspring);
        int new_fitness = harness.optimize(rand);
        if (best_fitness < new_fitness) {
          swap(best_offspring, offspring);
          best_fitness = new_fitness;
        }
        //cout << "Level: " << level << " New Fit: " << new_fitness << endl;
        /*
        if (fitness < fitnesses[level]){
          if (fitness < new_fitness) {
            // more efficient than assignment
            swap(solution, offspring);
            fitness = new_fitness;
          }
        } else {
          if (fitnesses[level] < new_fitness) {
            swap(solutions[level], offspring);
            fitnesses[level] = new_fitness;
          }
        }
        if (solutions[level] == solution) {
          break;
        }
        */
      }
      if (fitness < best_fitness) {
        fitness = best_fitness;
        solution = best_offspring;
      }
      // stored is best
      if (fitness <= fitnesses[level]) {
        fitness = fitnesses[level];
        solution = solutions[level];
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
