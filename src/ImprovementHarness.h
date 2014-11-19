/*
 * Neighborhood.h
 *
 *  Created on: Sep 10, 2014
 *      Author: goldman
 */

#ifndef IMPROVEMENTHARNESS_H_
#define IMPROVEMENTHARNESS_H_
#include "Evaluation.h"
#include "Neighborhood.h"
#include "RandIndex.hpp"
#include "Record.h"

#include <unordered_map>
using std::unordered_map;
#include <unordered_set>
using std::unordered_set;

class ImprovementHarness {
  vector<vector<size_t>> moves_;
  vector<unordered_set<size_t>> move_to_sub, sub_to_move;
  unordered_map<size_t, size_t> single_bit_moves;
  vector<int> delta;
  shared_ptr<GrayBox> evaluator;
  vector<bool> * solution;

  RandIndex<Random> options;
  void flip(size_t index);
  int make_move(size_t index);

  Record recording;
  // unverified

  int fitness, check_point_fitness;
  unordered_map<size_t, int> saved;
  unordered_set<size_t> flipped;

 public:
  ImprovementHarness(shared_ptr<GrayBox> evaluator_, size_t radius);
  virtual ~ImprovementHarness() = default;
  const vector<vector<size_t>>& moves() { return moves_; }
  const vector<size_t>& move(size_t index) { return moves_[index]; }
  int attach(vector<bool>* solution_);
  int optimize(Random & rand);
  int evaluate(const vector<bool>& solution_);

  void set_check_point();
  int modify(size_t bit);
  size_t modified() { return flipped.size(); }
  int revert();

  void dump_record(const Configuration& config, size_t run) {
    recording.dump(config, run);
  }

  // Controversial
  const vector<vector<size_t>>& epistasis() { return evaluator->epistasis(); }
  const shared_ptr<GrayBox>& evaler() { return evaluator; }
  int max_fitness() { return evaluator->max_fitness(); }
};

#endif /* IMPROVEMENTHARNESS_H_ */
