/*
 * Neighborhood.h
 *
 *  Created on: Sep 10, 2014
 *      Author: goldman
 */

#ifndef NEIGHBORHOOD_H_
#define NEIGHBORHOOD_H_
#include "Evaluation.h"

#include <unordered_map>
using std::unordered_map;
#include <unordered_set>
using std::unordered_set;

class Neighborhood {
  vector<vector<size_t>> moves_;
  vector<unordered_set<size_t>> move_to_sub, sub_to_move;
  unordered_map<size_t, vector<size_t>> bit_to_sub;
  vector<size_t> options, indexing;
  size_t useful;
  vector<float> delta;
  shared_ptr<GrayBox> evaluator;
  vector<bool> * solution;
  float fitness, check_point_fitness;
  unordered_map<size_t, float> saved;
  unordered_set<size_t> flipped;
  //vector<float> temp;
  unordered_map<size_t, size_t> single_bit_moves;
  void recurse(const unordered_map<size_t, unordered_set<size_t>>& graph,
               size_t v, unordered_set<size_t> & closed, vector<size_t> & prev,
               unordered_set<size_t> & prevopen, size_t radius);

 public:
  Neighborhood(shared_ptr<GrayBox> evaluator_, size_t radius);
  virtual ~Neighborhood() = default;
  const vector<vector<size_t>>& moves() { return moves_; }
  const vector<size_t>& move(size_t index) { return moves_[index]; }
  void reset() { useful = options.size(); }
  float attach(vector<bool>* solution_);
  float optimize(Random & rand);
  float make_bad(Random & rand);
  size_t untested() { return useful; }
  size_t rand_useful(Random& rand);
  void freeze(size_t index);
  void unfreeze(size_t index);
  void flip(size_t index, vector<bool>& solution);
  void flip(size_t index);
  float make_move(size_t index);
  void set_check_point();
  float modify(size_t bit);
  size_t modified() { return flipped.size(); }
  float revert();
  const float& get_delta(size_t index) { return delta[index]; }
  const unordered_set<size_t> & effected_moves(size_t sub) { return sub_to_move[sub]; }
  const unordered_set<size_t> & effected_subfunctions(size_t move) { return move_to_sub[move]; }
  void sanity_check();
};

#endif /* NEIGHBORHOOD_H_ */
