/*
 * Pyramid.h
 *
 *  Created on: Sep 13, 2014
 *      Author: goldman
 */

#ifndef PYRAMID_H_
#define PYRAMID_H_
#include "HillClimb.h"
#include "Evaluation.h"
#include "Crossover.h"

class Pyramid {
  vector<vector<bool>> solutions;
  vector<vector<size_t>> counts;
  vector<size_t> total_counts;
  vector<float> fitnesses;
  vector<bool> empty;
  unordered_map<size_t, unordered_set<size_t>> bit_to_bits;
  size_t length;
 public:
  Pyramid(Configuration& config);
  virtual ~Pyramid();
  float iterate(Random& rand, shared_ptr<GrayBox> evaluator, Neighborhood& neigbhors);
  float generate(Random& rand, shared_ptr<GrayBox> evaluator, Neighborhood& neigbhors);
  float grind(Random& rand, shared_ptr<GrayBox> evaluator, Neighborhood& neigbhors);
};

class Parallel {
  vector<std::pair<float, vector<bool>>> older, newer;
  size_t length;
 public:
  Parallel(Configuration& config);
  virtual ~Parallel() { };
  float grind(Random& rand, shared_ptr<GrayBox> evaluator, Neighborhood& neigbhors);

};

class Sharing {
  vector<std::pair<float, vector<bool>>> solutions;
  size_t length;
 public:
  Sharing(Configuration& config);
  virtual ~Sharing() { };
  float grind(Random& rand, shared_ptr<GrayBox> evaluator, Neighborhood& neigbhors);

};


class P3like {
  vector<vector<vector<bool>>> solutions;
  unordered_set<vector<bool>> seen;
  unordered_map<size_t, unordered_set<size_t>> bit_to_bits;
  vector<size_t> ordering;
  size_t length;
  hill_climb::pointer hc;
  string option;
  vector<size_t> successes, tries;
 public:
  P3like(Configuration& config);
  virtual ~P3like() { };
  float grind(Random& rand, shared_ptr<GrayBox> evaluator, Neighborhood& neigbhors);

};

class Lambda {
  vector<bool> solution;
  unordered_set<vector<bool>> seen;
  size_t length;
  hill_climb::pointer hc;
  size_t lambda;
  float fitness;
 public:
  Lambda(Configuration& config): length(config.get<size_t>("length")), hc(config.get<hill_climb::pointer>("hill_climber")), lambda(1), fitness(0) { }
  virtual ~Lambda() { };
  float grind(Random& rand, shared_ptr<GrayBox> evaluator, Neighborhood& neigbhors);
};

class DownUp {
  vector<bool> solution;
  size_t length;
  float fitness;
 public:
  DownUp(Configuration& config): length(config.get<size_t>("length")), fitness(0) { }
  virtual ~DownUp() { };
  float grind(Random& rand, shared_ptr<GrayBox> evaluator, Neighborhood& neighbors);
};

#endif /* PYRAMID_H_ */
