/*
 * RandomRestartHillClimber.h
 *
 *  Created on: Oct 15, 2014
 *      Author: goldman
 */

#ifndef RANDOMRESTARTHILLCLIMBER_H_
#define RANDOMRESTARTHILLCLIMBER_H_

#include "Optimizer.h"

class RandomRestartHC : public Optimizer {
 public:
  RandomRestartHC(Random& _rand, Configuration& _config, ImprovementHarness& _harness);
  int iterate() override;
  create_optimizer(RandomRestartHC);
};

#endif /* RANDOMRESTARTHILLCLIMBER_H_ */
