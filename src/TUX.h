// Brian Goldman

// Tournament Uniform Crossover is designed to be a parameter-less and simple
// method for combining r-bit hill climbing with uniform crossover to
// create a global search algorithm
#ifndef TUX_H_
#define TUX_H_

#include "Optimizer.h"
#include <random>

class TUX : public Optimizer {
  // Current list of solutions waiting for their partners
  vector<vector<bool>> tournament;
  vector<int> fitnesses;
  vector<bool> empty;

  vector<bool> offspring, best_offspring;
  std::uniform_int_distribution<int> rbit;

 public:
  TUX(Random& _rand, Configuration& _config, ImprovementHarness& _harness);
  int iterate() override;
  create_optimizer(TUX);
};

#endif /* TUX_H_ */
