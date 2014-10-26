// Brian Goldman

// File defining how to perform evaluation of a solution
// using multiple different evaluation functions.
// Each problem type defines its own class inherited from
// Evaluator and includes the create_evaluator macro.

#ifndef EVALUATION_H_
#define EVALUATION_H_
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <math.h>
#include <fstream>
#include <memory>
#include "Configuration.h"
#include "Util.h"

using std::vector;
using std::size_t;
using std::shared_ptr;

// This macro is used to define a factory function, simplifying the transition from
// configuration option to object capable of performing evaluation.
#define create_evaluator(name) static shared_ptr<GrayBox> create(Configuration& config, int run_number)\
{\
	return shared_ptr<GrayBox>(new name(config, run_number));\
}

// Base class to define the interface for evaluation functions
class GrayBox {
 protected:
  vector<vector<size_t>> epistasis_;
  size_t length_;
 public:
  GrayBox() : length_(0) { }
  virtual ~GrayBox() = default;
  int virtual evaluate(size_t subfunction, const vector<bool> & solution) = 0;
  const vector<vector<size_t>>& epistasis() {return epistasis_;}
  const size_t& length() { return length_; }
  int virtual max_fitness() = 0;
};

// Deceptive trap problem, where the string is composed of non-overlapping
// traps of "trap_size" number of bits.
class DeceptiveTrap : public GrayBox {
 public:
  DeceptiveTrap(Configuration& config, int run_number);
  int evaluate(size_t subfunction, const vector<bool> & solution) override;
  create_evaluator(DeceptiveTrap);
  int max_fitness() { return length_; }

 private:
  size_t trap_size;
};

// The Nearest Neighbor NK problem randomly generates
// a fitness landscape where the fitness of each bit
// relies on the k bits directly following it in the genome.
// Wraps around the end.
class NearestNeighborNK : public GrayBox {
  // data structure type used in finding the extremes of the problem
  using trimap=std::unordered_map<size_t,
  std::unordered_map<size_t,
  std::unordered_map<size_t, size_t>>>;

 public:
  size_t minimum;
  size_t maximum;
  vector<bool> best;
  vector<bool> worst;
  NearestNeighborNK(Configuration& config, int run_number);
  int evaluate(size_t subfunction, const vector<bool> & solution) override;
  create_evaluator(NearestNeighborNK);
  int max_fitness() { return maximum; }
 private:
  vector<vector<size_t> > table;
  size_t k;

  // These functions are involved in determining the maximimum achievable fitness
  // on the randomly generated landscape
  size_t chunk_fitness(trimap& known, size_t chunk_index, size_t a, size_t b);
  void int_into_bit(size_t src, vector<bool>& dest);
  size_t solve(vector<bool>& solution, bool maximize);
};

class UnrestrictedNK : public GrayBox {

 public:
  size_t maximum;
  UnrestrictedNK(Configuration& config, int run_number);
  int evaluate(size_t subfunction, const vector<bool> & solution) override;
  create_evaluator(UnrestrictedNK);
  int max_fitness() { return maximum; }
 private:
  vector<vector<size_t> > table;
  size_t k;
};

// This maximum satisfiability problem generates a set of random 3 literals
// clauses, where literals may be negated, assigning fitness equal to the
// number of clauses that contain at least one literal that evaluates to true.
// Unlike other MAXSAT, the problems generated here are always solvable.
class MAXSAT : public GrayBox {
 public:
  MAXSAT(Configuration& config, int run_number);
  int evaluate(size_t subfunction, const vector<bool> & solution) override;
  create_evaluator(MAXSAT);
  int max_fitness() { return total_weights; }
 private:
  vector<size_t> weights;
  size_t total_weights;
  vector<std::array<bool, 3>> signs;
  // Data structure used to select the negative signs on literals.
  // Ensures proper distribution of negated literals.
  vector<std::array<int, 3>> sign_options = { { {0, 0, 1} }, { {0, 1, 0} },
    { { 1, 0, 0} }, { {1, 0, 0} }, { {0, 1, 1} }, { {1, 1, 1} }, };
};


class MAXSAT_File : public GrayBox {
 public:
  MAXSAT_File(Configuration& config, int run_number);
  int evaluate(size_t subfunction, const vector<bool> & solution) override;
  create_evaluator(MAXSAT_File);
  int max_fitness() { return epistasis_.size(); }
 private:
  vector<vector<bool>> signs;
};

// This mapping is used to convert a problem's name into an instance
// of that Evaluator object
namespace evaluation {
using pointer=shared_ptr<GrayBox> (*)(Configuration &, int);
static std::unordered_map<string, pointer> lookup( {
    { "DeceptiveTrap", DeceptiveTrap::create },
    { "NearestNeighborNK", NearestNeighborNK::create },
    { "UnrestrictedNK", UnrestrictedNK::create },
    { "MAXSAT", MAXSAT::create },
    { "MAXSAT_File", MAXSAT_File::create },
});
}

#endif /* EVALUATION_H_ */
