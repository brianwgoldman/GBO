/*
 * Pyramid.cpp
 *
 *  Created on: Jun 26, 2013
 *      Author: goldman
 */

#include "BlackBoxP3.h"
#include <sstream>
using std::endl;

// Applies crossover between the passed in solution as each level
// of the pyramid
void BlackBoxP3::climb(vector<bool> & solution, int & fitness) {
  // attempts to add this solution to the base level
  add_unique(solution, 0);
  for (size_t level = 0; level < pops.size(); level++) {
    int prev = fitness;
    // Use population clusters and population solutions to make new solution
    pops[level].improve(rand, solution, fitness, harness);
    // add it to the next level if its a strict fitness improvement,
    // or configured to always add solutions
    if (prev < fitness) {
      add_unique(solution, level + 1);
    }
  }
}

// Add the solution to the specified level of the population, only if it is
// unique.  Returns true if added.
bool BlackBoxP3::add_unique(const vector<bool> & solution, size_t level) {
  if (seen.find(solution) == seen.end()) {
    if (pops.size() == level) {
      // Create new levels as necessary
      pops.push_back(Population(config));
    }
    // Add the solution and rebuild the tree
    pops[level].add(solution);
    pops[level].rebuild_tree(rand);
    // track that this solution is now in the pyramid
    seen.insert(solution);
    return true;
  }
  return false;
}

// Performs a full iteration of P3
int BlackBoxP3::iterate() {
  // generate a random solution
  rand_vector(rand, solution);
  int fitness = harness.evaluate(solution);
  // perform a local search hill climber
  fitness = BlackBoxImprovement(rand, solution, fitness, harness);
  // perform crossover with each level of the pyramid
  climb(solution, fitness);
  // P3 never "converges"
  return fitness;
}
