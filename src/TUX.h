/*
 * TUX.h
 *
 *  Created on: Oct 15, 2014
 *      Author: goldman
 */

#ifndef TUX_H_
#define TUX_H_

#include "Optimizer.h"
#include <random>

class TUX : public Optimizer {
  vector<vector<bool>> solutions;
  vector<int> fitnesses;
  vector<bool> empty;

  vector<bool> offspring, best_offspring;
  std::uniform_int_distribution<int> rbit;

 public:
  TUX(Random& _rand, Configuration& _config, ImprovementHarness& _harness);
  int iterate() override;create_optimizer(TUX);
};

#endif /* TUX_H_ */
