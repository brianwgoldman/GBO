// Brian Goldman

// Implements first improvement black box hill climbing.
// Used to perform timing comparisons and as part of Black Box P3.

#ifndef BLACKBOXHC_H_
#define BLACKBOXHC_H_

#include "Optimizer.h"

// Modifies in place "solution" until it reaches a local optima, then returns its new fitness.
int BlackBoxImprovement(Random& rand, vector<bool> & solution,
                        int initial_fitness, ImprovementHarness& harness);

// Wrapper optimization object which iteratively generates random solutions
// and improves them using hill climbing.
class BlackBoxHC : public Optimizer {
 public:
  BlackBoxHC(Random& _rand, Configuration& _config,
             ImprovementHarness& _harness);
  int iterate() override;
  create_optimizer(BlackBoxHC);
};

#endif /* BLACKBOXHC_H_ */
