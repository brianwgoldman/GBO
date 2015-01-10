/*
 * BlackBoxHC.h
 *
 *  Created on: Oct 21, 2014
 *      Author: goldman
 */

#ifndef BLACKBOXHC_H_
#define BLACKBOXHC_H_

#include "Optimizer.h"

int BlackBoxImprovement(Random& rand, vector<bool> & solution, int fitness, ImprovementHarness& harness);

class BlackBoxHC : public Optimizer {
 public:
  BlackBoxHC(Random& _rand, Configuration& _config,
             ImprovementHarness& _harness);
  int iterate() override;create_optimizer(BlackBoxHC);
};

#endif /* BLACKBOXHC_H_ */
