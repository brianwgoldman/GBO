// Brian Goldman

// This file aggregates optimization methods
// so that they can be chosen through configuration options

#ifndef OPTIMIZATIONCOLLECTION_H_
#define OPTIMIZATIONCOLLECTION_H_

#include "Optimizer.h"
#include "SAC.h"
#include "Pyramid.h"
#include "RandomRestartHC.h"
#include "BlackBoxHC.h"

#include <unordered_map>

namespace optimize {
// Renaming of the function pointer used to create new optimization methods
using pointer=shared_ptr<Optimizer> (*)(Random& rand, Configuration& config, ImprovementHarness& harness);

// Lookup table translates strings to function pointers
static std::unordered_map<string, pointer> lookup( {
  { "Pyramid", Pyramid::create },
  { "RandomRestartHC", RandomRestartHC::create },
  { "BlackBoxHC", BlackBoxHC::create },
  { "SAC", SAC::create },
});
}

#endif /* OPTIMIZATIONCOLLECTION_H_ */
