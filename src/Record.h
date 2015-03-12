// Brian Goldman

// Data structure for keeping track of how much time
// was used before each new level of fitness quality was
// reached.
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
  // Fitness / seconds pairs, worst to best fitness order
  vector<pair<int, double>> time_to_level;
  std::chrono::steady_clock::time_point start;
 public:
  Record() = default;
  virtual ~Record() = default;

  // Needs to be called before the start of optimization
  void start_clock() {
    start = std::chrono::steady_clock::now();
  }

  // Conditionally records that "fitness" was reached for the
  // first time at this time.
  void record(int fitness);

  // Output the record to a stream
  void dump(ostream& out) const;

  // Output the record to "dat_file" if not configured to "none"
  void dump(const Configuration& config) const;

  // The current best fitness reached. Causes problems
  // if nothing has been recorded.
  const int best_fitness_reached() const {
    return time_to_level.back().first;
  }

};

#endif /* RECORD_H_ */
