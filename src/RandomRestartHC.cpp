/*
 * RandomRestartHillClimber.cpp
 *
 *  Created on: Oct 15, 2014
 *      Author: goldman
 */

#include "RandomRestartHC.h"

RandomRestartHC::RandomRestartHC(Random& _rand, Configuration& _config, ImprovementHarness& _harness)
    : Optimizer(_rand, _config, _harness) {
}

int RandomRestartHC::iterate() {
  auto solution = rand_vector(rand, length);
  harness.attach(&solution);
  return harness.optimize(rand);
}
