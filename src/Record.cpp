// Brian Goldman

// Keep track of when fitness improvements occur, and output them
// to a file.
#include "Record.h"

double Record::elapsed() {
  auto current = std::chrono::steady_clock::now();
  return std::chrono::duration<double>(current - start).count();
}

// Checks if this fitness is better than the best so far. If it is,
// record it and the current amount of elapsed time.
void Record::record(int fitness) {
  if (time_to_level.size() == 0 or time_to_level.back().first < fitness) {
    time_to_level.emplace_back(fitness, elapsed());
  }
}

// Output the record in tsv 2 column format
void Record::dump(ostream& out) const {
  out << "Fitness\tSeconds" << std::endl;
  for (const auto& pair : time_to_level) {
    out << pair.first << "\t" << pair.second << std::endl;
  }
}

// If the configuration value for dat_file is not "none"
// output to the specified dat_file.
void Record::dump(const Configuration& config) const {
  string out_filename = config.get<string>("dat_file");
  if (out_filename != "none") {
    std::ofstream out(out_filename);
    dump(out);
  }
}
