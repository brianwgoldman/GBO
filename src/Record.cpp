/*
 * Record.cpp
 *
 *  Created on: Oct 29, 2014
 *      Author: goldman
 */

#include "Record.h"

void Record::record(int fitness) {
  if (time_to_level.size() == 0 or time_to_level.back().first < fitness) {
      auto current = std::chrono::steady_clock::now();
      auto elapsed = std::chrono::duration <double> (current - start).count();
      time_to_level.emplace_back(fitness, elapsed);
  }
}

void Record::dump(ostream& out) const {
  out << "Fitness\tSeconds" << std::endl;
  for (const auto& pair: time_to_level) {
    out << pair.first << "\t" << pair.second << std::endl;
  }
}

void Record::dump(const Configuration& config, size_t run) const {
  string out_filename = config.get<string>("dat_file");
  if (out_filename != "none") {
    //std::ofstream out(out_filename + "_" + std::to_string(run) + ".dat");
    std::ofstream out(out_filename);
    dump(out);
  }
}
