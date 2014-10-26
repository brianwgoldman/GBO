// Brian Goldman

// Defines the base class all optimization methods should inherit from.
// Provides tools for setting up configurable selection of optimization method.

#ifndef OPTIMIZER_H_
#define OPTIMIZER_H_

#include <memory>

#include "Util.h"
#include "ImprovementHarness.h"
#include "Configuration.h"

using std::shared_ptr;

// Macro used to create a function which returns new instances of the desired optimization method.
#define create_optimizer(name) static shared_ptr<Optimizer> create(Random& rand, Configuration& config, ImprovementHarness& harness)\
{\
	return shared_ptr<Optimizer>(new name(rand, config, harness));\
}

// Base class for optimization methods
class Optimizer {
 public:
  Optimizer(Random& _rand, Configuration& _config, ImprovementHarness& _harness)
      : rand(_rand),
        config(_config),
        length(_config.get<size_t>("length")),
        harness(_harness) {
  }
  virtual ~Optimizer() = default;
  // External tools interact with the optimizer by telling it to "iterate",
  // and the optimizer will return true as long as it can continue to improve by iterating.
  // As example, an iteration may perform a generation of evolution, with the optimizer returning
  // false when convergence is detected.
  virtual int iterate() = 0;

  virtual string finalize() {
    return string();
  }

 protected:
  // Tools useful to the actual optimization methods.
  Random& rand;
  Configuration& config;
  size_t length;
  ImprovementHarness& harness;
};

#endif /* OPTIMIZER_H_ */
