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
#include "RandSet.hpp"
#include "Record.h"

#include <unordered_map>
using std::unordered_map;
#include <unordered_set>
using std::unordered_set;

class ImprovementHarness {
  vector<vector<size_t>> moves_;
  vector<unordered_set<size_t>> move_to_sub, sub_to_move;
  vector<size_t> single_bit_moves;
  vector<int> delta;
  shared_ptr<GrayBox> evaluator;
  vector<bool> * solution;

  vector<RandSet<size_t, Random>> improvements;
  void flip_move(size_t move_index);
  int make_move(size_t move_index);

  Record& recording;

  int fitness, check_point_fitness;
  unordered_map<size_t, int> saved_delta;
  unordered_set<size_t> flipped;

 public:
  ImprovementHarness(shared_ptr<GrayBox> evaluator_, size_t radius,
                     Record& _recording);
  virtual ~ImprovementHarness() = default;
  int attach(vector<bool>* solution_);
  int optimize(Random & rand);
  int evaluate(const vector<bool>& solution_) {
    return evaluator->evaluate(solution_);
  }

  void set_check_point();
  int modify_bit(size_t bit);
  size_t modified() {
    return flipped.size();
  }
  int revert();

  const vector<vector<size_t>>& epistasis() const {
    return evaluator->epistasis();
  }
  int max_fitness() const {
    return evaluator->max_fitness();
  }
};

#endif /* IMPROVEMENTHARNESS_H_ */
