/*
 * Pyramid.h
 *
 *  Created on: Sep 13, 2014
 *      Author: goldman
 */

#ifndef PYRAMID_H_
#define PYRAMID_H_

#include "Evaluation.h"
#include "ImprovementHarness.h"
#include "Optimizer.h"
#include <unordered_map>
using std::unordered_map;
#include <unordered_set>
using std::unordered_set;

class Pyramid : public Optimizer {
  vector<vector<vector<bool>>> solutions;
  unordered_set<vector<bool>> seen;
  unordered_map<size_t, unordered_set<size_t>> bit_to_bits;
  vector<size_t> ordering;
  string option;
  vector<size_t> successes, tries;
  void sfx_tree(vector<vector<size_t>> & blocks);
 public:
  Pyramid(Random& _rand, Configuration& _config, ImprovementHarness& _harness);
  int iterate() override;
  create_optimizer(Pyramid);

};

#endif /* PYRAMID_H_ */
