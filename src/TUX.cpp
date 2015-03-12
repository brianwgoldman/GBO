// Tournament Uniform Crossover is meant to be a very simple algorithm
// for combining hamming ball hill climbing with global search
#include "TUX.h"

TUX::TUX(Random& _rand, Configuration& _config, ImprovementHarness& _harness)
    : Optimizer(_rand, _config, _harness),
      rbit(0, 1) {
  offspring.resize(length);
  best_offspring.resize(length);
}

// Performs a single iteration of TUX, which always starts
// with a new random solution being improved via r-bit hamming ball
// hill climbing. It can also include uniform crossover depending
// on the current state of search.
int TUX::iterate() {
  rand_vector(rand, solution);
  harness.attach(&solution);
  auto fitness = harness.optimize(rand);
  // Scan up the tournament for opponents
  for (size_t level = 0; level < tournament.size(); level++) {
    // There is no opponent yet, so wait for one to be made later
    if (empty[level]) {
      // more efficient than assignment
      tournament[level].swap(solution);
      fitnesses[level] = fitness;
      empty[level] = false;
      return fitness;
    } else {
      // Generate 2**(level+1) solutions using uniform crossover
      // and keep the best.
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
        // Apply r-bit hamming ball hill climbing
        harness.attach(&offspring);
        int new_fitness = harness.optimize(rand);
        if (best_fitness < new_fitness) {
          best_offspring.swap(offspring);
          best_fitness = new_fitness;
        }
      }
      // Solution becomes best offspring
      if (fitness < best_fitness) {
        fitness = best_fitness;
        solution.swap(best_offspring);
      }
      // if other parent is better than solution
      if (fitness < fitnesses[level]) {
        fitness = fitnesses[level];
        solution.swap(tournament[level]);
      }
      // solution is now the best, so empty the level
      empty[level] = true;
    }
  }
  empty.push_back(false);
  fitnesses.push_back(fitness);
  tournament.push_back(solution);
  return fitness;
}
