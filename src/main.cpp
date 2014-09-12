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
#include "HillClimb.h"
#include "Crossover.h"
#include "Neighborhood.h"
using namespace std;

bool compare_tuples(const vector<size_t>& x, const vector<size_t>& y) {
  if (x.size() != y.size()) {
    return x.size() < y.size();
  }
  for (size_t i=0; i < x.size(); i++) {
    if (x[i] != y[i]) {
      return x[i] < y[i];
    }
  }
  throw "SHIT";
  return true;
}

int main(int argc, char * argv[]) {
  /*
  auto start = chrono::steady_clock::now();
  cout << "system_clock" << endl;
  cout << chrono::system_clock::period::num << endl;
  cout << chrono::system_clock::period::den << endl;
  cout << "steady = " << boolalpha << chrono::system_clock::is_steady << endl << endl;

  cout << "high_resolution_clock" << endl;
  cout << chrono::high_resolution_clock::period::num << endl;
  cout << chrono::high_resolution_clock::period::den << endl;
  cout << "steady = " << boolalpha << chrono::high_resolution_clock::is_steady << endl << endl;

  cout << "steady_clock" << endl;
  cout << chrono::steady_clock::period::num << endl;
  cout << chrono::steady_clock::period::den << endl;
  cout << "steady = " << boolalpha << chrono::steady_clock::is_steady << endl << endl;
  auto end = chrono::steady_clock::now();
  auto diff = end - start;
  cout << chrono::duration <double, milli> (diff).count() << " ms" << endl;
  */

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
  auto gray_box = problem(config, config.get<int>("problem_seed"));
  auto climb = config.get<hill_climb::pointer>("hill_climber");
  Neighborhood neighbors(gray_box, config.get<int>("radius"));
  cout << "Start" << endl;
  auto start = chrono::steady_clock::now();
  /*
  Neighborhood neighbors(gray_box, config.get<int>("radius"));
  auto moves = neighbors.moves();
  sort(moves.begin(), moves.end(), compare_tuples);
  cout << moves.size() << endl;
  */
  /*
  cout << "Subfunctions" << endl;
  for (const auto& subfunction: gray_box->epistasis()) {
    for (const auto& x: subfunction) {
      cout << x << ",";
    }
    cout << endl;
  }
  cout << "Moves" << endl;
  for (const auto& move: moves) {
    for (const auto& x: move) {
      cout << x <<", ";
    }
    cout << endl;
  }
  */
  //*
  float average = 0;
  float best = 0;
  for (size_t i = 0; i < config.get<size_t>("runs"); i++) {
    auto solution = rand_vector(rand, config.get<int>("length"));
    auto fitness = climb(rand, solution, gray_box, neighbors);
    float temp = 0;
    for (size_t i =0; i < gray_box->epistasis().size(); i++) {
      temp += gray_box->evaluate(i, solution);
    }
    if (temp != fitness) {
      cout << "ERROR " << temp << " " << fitness << endl;
    }
    //auto p2 = rand_vector(rand, config.get<int>("length"));
    //climb(rand, p2, gray_box);
    //vector<bool> offspring;
    //fitness = sfx(rand, solution, p2, offspring, gray_box);
    average += fitness;
    if (best < fitness) {
      best = fitness;
    }
  }

  // best /= gray_box->epistasis().size();
  // average /= gray_box->epistasis().size();
  cout << "Average fitness: " << average / config.get<int>("runs") << " Best: " << best << endl;
  //*/
  auto end = chrono::steady_clock::now();
  cout << chrono::duration <double> (end - start).count() << " seconds" << endl;
  return 0;
}


