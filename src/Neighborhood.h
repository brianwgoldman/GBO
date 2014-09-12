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
  vector<size_t> options, indexing;
  size_t useful;

  void recurse(const unordered_map<size_t, unordered_set<size_t>>& graph,
               size_t v, unordered_set<size_t> & closed, vector<size_t> & prev,
               unordered_set<size_t> & prevopen, size_t radius);

 public:
  Neighborhood(shared_ptr<GrayBox> evaluator, size_t radius);
  virtual ~Neighborhood() = default;
  const vector<vector<size_t>>& moves() { return moves_; }
  const vector<size_t>& move(size_t index) { return moves_[index]; }
  void reset() { useful = options.size(); }
  size_t untested() { return useful; }
  size_t rand_useful(Random& rand);
  void freeze(size_t index);
  void unfreeze(size_t index);
  void flip(size_t index, vector<bool>& solution);
  const unordered_set<size_t> & effected_moves(size_t sub) { return sub_to_move[sub]; }
  const unordered_set<size_t> & effected_subfunctions(size_t move) { return move_to_sub[move]; }
  void sanity_check();
};

#endif /* NEIGHBORHOOD_H_ */
