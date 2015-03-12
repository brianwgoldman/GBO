// Brian Goldman

// Very basic "main" which drives the rest of the files.
// Built upon command line configuration which allows you to
// specify things like "problem", "optimizer", "length", etc.
// Performs a single run stopping at timeout or global optimum.

// ---------------- Compilation ----------------------------
// In Release/ and Debug/ there are makefiles to compile the source
// To perform experiments, call "make GBO" in the Release directory.
// For our experiments we used G++ 4.8.2.

// ---------------- Example Usage ----------------------------
// Release/GBO config/default.cfg -verbosity 2
// This will run the "default" configurations with increase output.
// Any argument without a "-" is considered a configuration file.
// Any argument with a "-" is treated as a configuration key, followed
// by its value.
// Release/GBO config/default.cfg -problem NearestNeighborNKQ -length 100 -optimizer BlackBoxP3 -minutes 0.1
// This will perform optimization on NearestNeighborNKQ with N=100 and k set to the default
// using Black Box P3 to perform optimization. Search will terminate after 6 seconds or when the global
// optimum is reached. To test Gray Box P3, replace "BlackBoxP3" with "Pyramid"
#include <iostream>
#include <vector>
#include <chrono>
#include "Configuration.h"
#include "Evaluation.h"
#include "ImprovementHarness.h"
#include "OptimizationCollection.h"
using namespace std;

int main(int argc, char * argv[]) {
  Configuration config;
  config.parse(argc, argv);

  // Set up the random number seed either using the configuration value
  // or the computers built in seed generator.
  Random rand;
  int seed = config.get<int>("seed");
  if (seed == -1) {
    std::random_device rd;
    seed = rd();
    config.set("seed", seed);
  }
  // save configuration if you need to do it all again
  config.dump();
  rand.seed(seed);
  // Control how much is output to the screen
  int verbosity = config.get<int>("verbosity");
  Record recording;

  auto problem = config.get<evaluation::pointer>("problem");
  auto gray_box = problem(config);
  auto optimizer = config.get<optimize::pointer>("optimizer");

  // Initially the best is the lowest possible integer
  int best = numeric_limits<int>::min();
  double elapsed = 0;

  // Start of timing
  auto start = chrono::steady_clock::now();
  ImprovementHarness harness(gray_box, config.get<int>("radius"), recording);

  // Construct the optimization class, e.g. P3
  auto solver = optimizer(rand, config, harness);
  do {
    auto fitness = solver->iterate();
    if (best < fitness) {
      best = fitness;
      if (verbosity > 1) {
        cout << endl << "Best: " << best;
      }
    }
    // outputs a dot for each iteration.
    if (verbosity > 1) {
      cout << ".";
      cout.flush();
    }
    auto end = chrono::steady_clock::now();
    elapsed = chrono::duration<double, ratio<60>>(end - start).count();
    // Keep going while there is time left and you haven't found the global optimum.
  } while (elapsed < config.get<float>("minutes")
      and best < gray_box->max_fitness());
  // Output any recording information as directed by the configuration.
  recording.dump(config);

  cout << endl << "----------------Minutes: " << elapsed << " Best: " << best
       << endl;
  return 0;
}

