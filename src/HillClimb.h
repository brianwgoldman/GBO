// Brian Goldman

// There are many ways to perform local search, and this
// file contains the implementation for a lot of configuration options.
// In all published experiments (so far) "first_improvement" was used,
// but other methods were informally tested.

#ifndef HILLCLIMB_H_
#define HILLCLIMB_H_

#include <unordered_map>
#include <unordered_set>
#include "Evaluation.h"
#include "Configuration.h"
#include "Util.h"
#include "Neighborhood.h"

namespace hill_climb {
// Creates a type name for pointers to hill climbing functions
using pointer=float (*)(Random & rand, vector<bool> & solution, shared_ptr<GrayBox> evaluator, Neighborhood& neighborhood);

// Each time an improvement is found, it is immediately kept, with search progressing
// from the modified solution
float first_improvement(Random & rand, vector<bool> & solution, shared_ptr<GrayBox> evaluator, Neighborhood& neighborhood);


float first_alternative(Random & rand, vector<bool> & solution, shared_ptr<GrayBox> evaluator, Neighborhood& neighborhood);

float first_memory(Random & rand, vector<bool> & solution, shared_ptr<GrayBox> evaluator, Neighborhood& neighborhood);

float neighbor_memory(Random & rand, vector<bool> & solution, shared_ptr<GrayBox> evaluator, Neighborhood& neighborhood);

// Mapping used to convert configuration string names to actual function pointers
static std::unordered_map<string, pointer> lookup( {
  { "first_improvement", first_improvement },
  { "first_alternative", first_alternative },
  { "first_memory", first_memory },
  { "neighbor_memory", neighbor_memory },
});
}

#endif /* HILLCLIMB_H_ */
