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
#include <array>
#include "Configuration.h"
#include "Util.h"

#define PI 3.14159265

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
 public:
  GrayBox() = default;
  virtual ~GrayBox() = default;
  float virtual evaluate(size_t subfunction, const vector<bool> & solution) = 0;
  const vector<vector<size_t>>& epistasis() {return epistasis_;}
};

// Deceptive trap problem, where the string is composed of non-overlapping
// traps of "trap_size" number of bits.
class DeceptiveTrap : public GrayBox {
 public:
  DeceptiveTrap(Configuration& config, int run_number);
  float evaluate(size_t subfunction, const vector<bool> & solution) override;
  create_evaluator(DeceptiveTrap);

 private:
  size_t trap_size;
  int precision;
};
/*
// Deceptive Step trap problem, similar to the Deceptive Trap problem,
// except each trap includes configurable plataeus of "step_size" bits.
class DeceptiveStepTrap : public GrayBox {
 public:
  DeceptiveStepTrap(Configuration& config, int run_number)
      : trap_size(config.get<int>("trap_size")),
        step_size(config.get<int>("step_size")),
        precision(config.get<int>("precision")) {
    offset = (trap_size - step_size) % step_size;
  }
  float evaluate(size_t subfunction, const vector<bool> & solution) override;
  create_evaluator(DeceptiveStepTrap);

 private:
  int trap_size;
  int step_size;
  int offset;
  int precision;

};

class Bipolar : public GrayBox {
 public:
  Bipolar(Configuration& config, int run_number)
      : trap_size(config.get<int>("trap_size")),
        precision(config.get<int>("precision")) {
  }
  float evaluate(size_t subfunction, const vector<bool> & solution) override;
  create_evaluator(Bipolar);

private:
  int trap_size;
  int precision;
};
*/

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
  float evaluate(size_t subfunction, const vector<bool> & solution) override;
  create_evaluator(NearestNeighborNK);

 private:
  vector<vector<size_t> > table;
  size_t k;
  int length;
  int precision;

  // These functions are involved in determining the maximimum achievable fitness
  // on the randomly generated landscape
  size_t chunk_fitness(trimap& known, size_t chunk_index, size_t a, size_t b);
  void int_into_bit(size_t src, vector<bool>& dest);
  size_t solve(vector<bool>& solution, bool maximize);
};

// This maximum satisfiability problem generates a set of random 3 literals
// clauses, where literals may be negated, assigning fitness equal to the
// number of clauses that contain at least one literal that evaluates to true.
// Unlike other MAXSAT, the problems generated here are always solvable.
class MAXSAT : public GrayBox {
 public:
  MAXSAT(Configuration& config, int run_number);
  float evaluate(size_t subfunction, const vector<bool> & solution) override;
  create_evaluator(MAXSAT);
 private:
  int precision;
  vector<std::array<bool, 3>> signs;
  // Data structure used to select the negative signs on literals.
  // Ensures proper distribution of negated literals.
  vector<std::array<int, 3>> sign_options = { { {0, 0, 1} }, { {0, 1, 0} },
    { { 1, 0, 0} }, { {1, 0, 0} }, { {0, 1, 1} }, { {1, 1, 1} }, };
};
/*
// The Ising Spin Glass problem is defined by a 2d toroidal grid
// of node interactions.  The goal is to set the "spin" of each node
// to minimize interaction terms.  This problem requires external tools
// to create and define the bounds of each problem.
class IsingSpinGlass : public GrayBox {
 public:
  IsingSpinGlass(Configuration& config, int run_number);
  float evaluate(size_t subfunction, const vector<bool> & solution) override;
  create_evaluator(IsingSpinGlass);

 private:
  int length;
  int precision;
  int min_energy;
  float span;
  // used to quickly convert a bit to a sign.
  std::array<int, 2> bit_to_sign = { { -1, 1 } };
  // each spin is composed of 3 numbers: index, index, sign
  vector<std::array<int, 3>> spins;
};
*/
// This mapping is used to convert a problem's name into an instance
// of that Evaluator object
namespace evaluation {
using pointer=shared_ptr<GrayBox> (*)(Configuration &, int);
static std::unordered_map<string, pointer> lookup( {
    { "DeceptiveTrap", DeceptiveTrap::create },
    //{ "DeceptiveStepTrap", DeceptiveStepTrap::create },
    //{ "Bipolar", Bipolar::create },
    { "NearestNeighborNK", NearestNeighborNK::create },
    { "MAXSAT", MAXSAT::create },
    //{ "IsingSpinGlass", IsingSpinGlass::create },
});
}

#endif /* EVALUATION_H_ */
