/*
 * main.cpp
 *
 *  Created on: Sep 4, 2014
 *      Author: goldman
 */

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
  int verbosity = config.get<int>("verbosity");
  Record recording;

  auto problem = config.get<evaluation::pointer>("problem");
  auto gray_box = problem(config);
  auto optimizer = config.get<optimize::pointer>("optimizer");

  int best = numeric_limits<int>::min();
  double elapsed = 0;

  // Start of timing
  auto start = chrono::steady_clock::now();
  ImprovementHarness harness(gray_box, config.get<int>("radius"), recording);

  auto solver = optimizer(rand, config, harness);
  do {
    auto fitness = solver->iterate();
    if (best < fitness) {
      best = fitness;
      if (verbosity > 1) {
        cout << endl << "Best: " << best;
      }
    }
    if (verbosity > 1) {
      cout << ".";
      cout.flush();
    }
    auto end = chrono::steady_clock::now();
    elapsed = chrono::duration<double, ratio<60>>(end - start).count();
  } while (elapsed < config.get<float>("minutes")
      and best < gray_box->max_fitness());
  recording.dump(config);

  cout << endl << "----------------Time: " << elapsed << " Best: " << best
       << endl;
  return 0;
}

