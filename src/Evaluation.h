// Brian Goldman

// File defining how to perform evaluation of a solution
// using multiple different evaluation functions.
// Each problem type defines its own class inherited from
// GrayBox and includes the create_graybox macro to allow
// for factory generation.

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
#define create_graybox(name) static shared_ptr<GrayBox> create(Configuration& config)\
{\
	return shared_ptr<GrayBox>(new name(config));\
}

// Base class to define the interface for evaluation functions
class GrayBox {
 protected:
  vector<vector<size_t>> epistasis_;
  size_t length_;
 public:
  GrayBox()
      : length_(0) {
  }
  virtual ~GrayBox() = default;
  int virtual evaluate(size_t subfunction, const vector<bool> & solution) = 0;
  int virtual evaluate(const vector<bool> & solution);
  const vector<vector<size_t>>& epistasis() {
    return epistasis_;
  }
  const size_t& length() {
    return length_;
  }
  int virtual max_fitness() = 0;
};

// Deceptive trap problem, where the string is composed of non-overlapping
// traps of "trap_size" number of bits.
class DeceptiveTrap : public GrayBox {
 public:
  DeceptiveTrap(Configuration& config);
  int evaluate(size_t subfunction, const vector<bool> & solution) override;
  create_graybox(DeceptiveTrap);
  int max_fitness() {return length_;}

private:
  size_t trap_size;
};

// Fitness landscape with N subfunctions, each reading from K+1
// variables to look up a function value from a randomly generated table,
// such that table entries must be 0 through 2**k - 1
class UnrestrictedNKQ : public GrayBox {

 public:
  UnrestrictedNKQ(Configuration& config);
  int evaluate(size_t subfunction, const vector<bool> & solution) override;
  create_graybox(UnrestrictedNKQ);
  int max_fitness() {return maximum;}
private:
  vector<vector<size_t> > table;
  size_t k;
  size_t maximum;
};

// The Nearest Neighbor NK problem randomly generates
// a fitness landscape where the fitness of each bit
// relies on the k bits directly following it in the genome.
// Wraps around the end. Function values are restricted to
// the integers 0 through 2**k - 1
class NearestNeighborNKQ : public GrayBox {
  // data structure type used in finding the extremes of the problem
  using trimap=std::unordered_map<size_t,
  std::unordered_map<size_t,
  std::unordered_map<size_t, size_t>>>;

 public:
  NearestNeighborNKQ(Configuration& config);
  int evaluate(size_t subfunction, const vector<bool> & solution) override;
  create_graybox(NearestNeighborNKQ);
  int max_fitness() {return maximum;}
private:
  vector<vector<size_t> > table;
  size_t k;
  size_t maximum;
  // These three are basically useless
  size_t minimum;
  vector<bool> best;
  vector<bool> worst;

  // These functions are involved in determining the maximum achievable fitness
  // on the randomly generated landscape
  size_t chunk_fitness(trimap& known, size_t chunk_index, size_t a, size_t b);
  void int_into_bit(size_t src, vector<bool>& dest);
  size_t solve(vector<bool>& solution, bool maximize);
};

// Read from a file, and ising spin glass is defined by a collections of edges
// and their integer weights, with the goal to be to maximize the result
// of xi * sij * xj, where xi and xj are either -1 or +1 and sij is the
// edge weight connecting xi and xj.
class IsingSpinGlass : public GrayBox {
 public:
  IsingSpinGlass(Configuration& config);
  int evaluate(size_t subfunction, const vector<bool> & solution) override;
  create_graybox(IsingSpinGlass);
  int max_fitness() {return maximum;}

private:
  int maximum;
  // used to quickly convert a bit to a sign.
  std::array<int, 2> bit_to_sign = { { -1, 1 } };
  // the sign of the edge
  vector<int> spins;
};

// This maximum satisfiability problem generates a set of random 3 literals
// clauses, where literals may be negated, assigning fitness equal to the
// number of clauses that contain at least one literal that evaluates to true.
// Unlike other MAXSAT, the problems generated here are always solvable.
class MAXSAT : public GrayBox {
 public:
  MAXSAT(Configuration& config);
  int evaluate(size_t subfunction, const vector<bool> & solution) override;
  create_graybox(MAXSAT);
  int max_fitness() {return epistasis_.size();}
protected:
  MAXSAT() = default;
private:
  vector<std::array<bool, 3>> signs;
  // Data structure used to select the negative signs on literals.
  // Ensures proper distribution of negated literals.
  const vector<std::array<int, 3>> sign_options = { { {0, 0, 1}}, { {0, 1, 0}},
    { { 1, 0, 0}}, { {1, 0, 0}}, { {0, 1, 1}}, { {1, 1, 1}},};
};

// Reads MAXSAT problems from a file.
class MAXSAT_File : public GrayBox {
 public:
  MAXSAT_File(Configuration& config);
  int evaluate(size_t subfunction, const vector<bool> & solution) override;
  create_graybox(MAXSAT_File);
  int max_fitness() {return epistasis_.size();}
private:
  vector<vector<bool>> signs;
};

// Reads MAXCUT problems from a file.
class MAXCUT_File : public GrayBox {
 public:
  MAXCUT_File(Configuration& config);
  int evaluate(size_t subfunction, const vector<bool> & solution) override;
  create_graybox(MAXCUT_File);
  int max_fitness() {return maximum;}
private:
  vector<int> weights;
  int maximum;
};

// This mapping is used to convert a problem's name into an instance
// of that Evaluator object
namespace evaluation {
using pointer=shared_ptr<GrayBox> (*)(Configuration &);
static std::unordered_map<string, pointer> lookup({
  { "DeceptiveTrap", DeceptiveTrap::create },
  { "NearestNeighborNKQ", NearestNeighborNKQ::create },
  { "UnrestrictedNKQ", UnrestrictedNKQ::create },
  { "IsingSpinGlass", IsingSpinGlass::create },
  { "MAXSAT", MAXSAT::create },
  { "MAXSAT_File", MAXSAT_File::create },
  { "MAXCUT_File", MAXCUT_File::create },
});
}

#endif /* EVALUATION_H_ */
