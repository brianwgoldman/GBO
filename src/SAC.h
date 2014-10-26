/*
 * SAC.h
 *
 *  Created on: Oct 15, 2014
 *      Author: goldman
 */

#ifndef SAC_H_
#define SAC_H_

#include "Optimizer.h"
#include <random>

class SAC : public Optimizer {
  vector<vector<bool>> solutions;
  vector<int> fitnesses;
  vector<bool> empty;

  std::bernoulli_distribution coin;

 public:
  SAC(Random& _rand, Configuration& _config, ImprovementHarness& _harness);
  int iterate() override;
  create_optimizer(SAC);
};

#endif /* SAC_H_ */
