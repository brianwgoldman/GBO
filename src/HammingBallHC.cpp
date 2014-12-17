/*
 * HammingBall.cpp
 *
 *  Created on: Oct 15, 2014
 *      Author: goldman
 */

#include "HammingBallHC.h"

HammingBallHC::HammingBallHC(Random& _rand, Configuration& _config,
                             ImprovementHarness& _harness)
    : Optimizer(_rand, _config, _harness) {
}

int HammingBallHC::iterate() {
  rand_vector(rand, solution);
  harness.attach(&solution);
  return harness.optimize(rand);
}
