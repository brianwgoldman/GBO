/*
 * Record.h
 *
 *  Created on: Oct 29, 2014
 *      Author: goldman
 */

#ifndef RECORD_H_
#define RECORD_H_
#include <vector>
using std::vector;
using std::pair;

#include <chrono>
#include <iostream>
using std::ostream;

#include "Configuration.h"

class Record {
  vector<pair<int, double>> time_to_level;
  std::chrono::steady_clock::time_point start;
 public:
  Record() = default;
  virtual ~Record() = default;

  void start_clock() {
    start = std::chrono::steady_clock::now();
  }

  void record(int fitness);

  void dump(ostream& out) const;

  void dump(const Configuration& config) const;
  // controversial
  const vector<pair<int, double>>& progression() const {
    return time_to_level;
  }

  const int best_fitness_reached() const {
    return time_to_level.back().first;
  }


};

#endif /* RECORD_H_ */
