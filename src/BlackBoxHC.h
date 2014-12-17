/*
 * BlackBoxHC.h
 *
 *  Created on: Oct 21, 2014
 *      Author: goldman
 */

#ifndef BLACKBOXHC_H_
#define BLACKBOXHC_H_

#include "Optimizer.h"

class BlackBoxHC : public Optimizer {
  vector<int> options;
 public:
  BlackBoxHC(Random& _rand, Configuration& _config,
             ImprovementHarness& _harness);
  int iterate() override;create_optimizer(BlackBoxHC);
};

#endif /* BLACKBOXHC_H_ */
