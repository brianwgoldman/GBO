// Brian Goldman

// The Black Box version of P3. Does not use any partial
// evaluations or interaction information.
// Included for timing comparison purposes.
#ifndef BLACKBOXP3_H_
#define BLACKBOXP3_H_

#include "BlackBoxPopulation.h"
#include "Util.h"
#include "BlackBoxHC.h"
#include "Configuration.h"
#include "Optimizer.h"
#include "ImprovementHarness.h"

// Implements the Optimizer interface
class BlackBoxP3 : public Optimizer {
 public:
  BlackBoxP3(Random& _rand, Configuration& _config,
             ImprovementHarness& _evaluator)
      : Optimizer(_rand, _config, _evaluator) {
  }
  // Peforms one complete iteration of
  // * random generation
  // * hill climbing
  // * crossover with each level of the pyramid (climb function)
  int iterate() override;
  create_optimizer(BlackBoxP3);

private:
  // Iteratively improves the solution using the pyramid of populations
  // leverages the Population class extensively
  void climb(vector<bool> & solution, int & fitness);

  // Handles adding a solution to the population.
  // Returns true if it was unique and therefore added.
  bool add_unique(const vector<bool> & solution, size_t level);

  // the pyramid of populations
  vector<Population> pops;
  // keeps track of the set of solutions in the pyramid
  std::unordered_set<vector<bool>> seen;
};

#endif /* BLACKBOXP3_H_ */
