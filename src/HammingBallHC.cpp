// Brian Goldman

// Wrapper for using the Improvement Harness to perform repeated
// local search
#include "HammingBallHC.h"

HammingBallHC::HammingBallHC(Random& _rand, Configuration& _config,
                             ImprovementHarness& _harness)
    : Optimizer(_rand, _config, _harness) {
}

// Generate a random solution and use the Improvement Harness
// to bring it to a local optima.
int HammingBallHC::iterate() {
  rand_vector(rand, solution);
  harness.attach(&solution);
  return harness.optimize(rand);
}
