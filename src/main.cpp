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
  rand.seed(seed);
  auto problem = config.get<evaluation::pointer>("problem");
  vector<int> missing;
  size_t solved = 0;
  vector<double> solved_time;
  for (size_t run=0; run < config.get<size_t>("runs"); run++) {
    auto gray_box = problem(config, run);

    ImprovementHarness neighbors(gray_box, config.get<int>("radius"));
    int best = -1;
    cout << "Start" << endl;
    auto start = chrono::steady_clock::now();
    auto solver = config.get<optimize::pointer>("optimizer")(rand, config, neighbors);

    double elapsed = 0;
    do {
      auto fitness = solver->iterate();
      if (best < fitness) {
        best = fitness;
        cout << endl << "Best: " << best;
      }
      cout << ".";
      cout.flush();
      auto end = chrono::steady_clock::now();
      elapsed = chrono::duration <double, ratio<60>> (end - start).count();
    } while (elapsed < config.get<float>("minutes") and best < gray_box->max_fitness());
    if (best >= gray_box->max_fitness()) {
      solved++;
    }
    neighbors.dump_record(config, run);
    cout << endl << "----------------Time: " << elapsed << " total solved: " << solved << " of " << run + 1 << endl;
    missing.push_back(gray_box->max_fitness() - best);
    solved_time.push_back(elapsed);
    for (auto fit: missing) {
      cout << fit << ", ";
    }
    cout << endl;
    for (auto t: solved_time) {
      cout << t << ", ";
    }
    cout << endl;
  }
  double total_time = 0;
  for (auto t: solved_time) {
    total_time += t;
  }
  cout << "Time: " << total_time << endl;
  return 0;
}


