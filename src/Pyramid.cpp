/*
 * Pyramid.cpp
 *
 *  Created on: Sep 13, 2014
 *      Author: goldman
 */

#include "Pyramid.h"

using namespace std;

Pyramid::Pyramid(Random& _rand, Configuration& _config, ImprovementHarness& _harness)
        : Optimizer(_rand, _config, _harness)  {
  ordering.resize(length);
  iota(ordering.begin(), ordering.end(), 0);
  option = config.get<string>("option");
  successes.resize(length, 0);
  tries.resize(length, 0);
}

// TODO Idea about using subfunctions to come up with random colorings of the graph
void Pyramid::sfx_tree(vector<vector<size_t>> & blocks) {
  for (size_t i=0; i < length; i++) {
    // creates vectors of size 1 with value i
    blocks.emplace_back(1, i);
  }
  vector<size_t> options(harness.epistasis().size());
  iota(options.begin(), options.end(), 0);
  vector<size_t> bit_to_block(length);
  iota(bit_to_block.begin(), bit_to_block.end(), 0);
  shuffle(options.begin(), options.end(), rand);
  // for each subfunction in a random order
  for (const auto& sub: options) {
    unordered_set<size_t> block_numbers;
    // for each bit in the subfunction
    for (const auto& bit: harness.epistasis()[sub]) {
      block_numbers.insert(bit_to_block[bit]);
    }
    // a merge is necessary
    if (block_numbers.size() > 1) {
      size_t largest = 0;
      // start a new empty block
      blocks.push_back(vector<size_t>(0));
      // the same bit cannot appear in two blocks, so just combine
      for (const auto& block_number: block_numbers) {
        if (blocks[block_number].size() > largest) {
          largest = blocks[block_number].size();
        }
        for (const auto& bit: blocks[block_number]) {
          blocks.back().push_back(bit);
        }
      }
      if (false and blocks.back().size() >= length/2) {
      //if (false and largest >= length/2) {
        blocks.pop_back();
      } else {
        // assign moved bits to new block
        for (const auto& bit: blocks.back()) {
          bit_to_block[bit] = blocks.size() - 1;
        }
      }
    }
  }

  if (blocks.back().size() == length) {
    blocks.pop_back();
  }
  blocks.erase(blocks.begin(), blocks.begin()+length);
}


int Pyramid::iterate() {
  /*
  for (size_t level=0; level < solutions.size(); level++) {
    cout << level << " " << solutions[level].size() << " | ";
  }
  cout << endl;
  */
  auto solution = rand_vector(rand, length);
  harness.attach(&solution);
  auto fitness = harness.optimize(rand);
  bool improved = true;
  size_t donations=0, sizes=0;
  for (size_t level=0; level < solutions.size(); level++) {
    //cout << "Level: " << level << " fit: " << fitness << endl;
    if (improved) {
      if (seen.count(solution) == 0) {
        solutions[level].push_back(solution);
        seen.insert(solution);
      } else {
        //cout << "Prevented midlevel" << endl;
      }

      improved = false;
      //print(solution);
    }
    vector<vector<size_t>> blocks;
    sfx_tree(blocks);
    //shuffle(blocks.begin(), blocks.end(), rand);
    //sort(blocks.begin(), blocks.end(), [](const vector<size_t>& b1, const vector<size_t>& b2){return b1.size() > b2.size();});
    unordered_map<size_t, size_t> freq;
    size_t sum=0;
    for (const auto& block: blocks) {
      sum += block.size();
      for (const auto& bit: block) {
        //cout << bit << ", ";
        freq[bit]++;
      }
      //cout << endl;
    }

    // uniform_int_distribution<size_t> rand_donor(0, solutions[level].size()-1);
    vector<size_t> options(solutions[level].size());
    iota(options.begin(), options.end(), 0);
    shuffle(ordering.begin(), ordering.end(), rand);
    size_t no_change=0, attempts=0;
    //for (size_t index=0; index < solution.size(); index++) {
    for (size_t index=0; index < blocks.size(); index++) {
      //auto copy = solution;
      size_t limit = options.size();
      harness.set_check_point();
      while (limit > 0 and harness.modified() == 0) {
        attempts++;
        size_t choice = uniform_int_distribution<size_t>(0, limit-1)(rand);
        size_t donor = options[choice];
        swap(options[choice], options[limit-1]);
        limit--;
        //cout << "Crossing" << endl;
        //print(solutions[level][donor]);
        //print(solution);
        //for (size_t bit: bit_to_bits[ordering[index]]) {
        for (size_t bit: blocks[index]) {
          //cout << bit << ", ";
          if (solution[bit] != solutions[level][donor][bit]) {
            harness.modify(bit);
          }
        }
      }
      if (harness.modified() == 0) {
        no_change++;
      }
      //cout << endl;
      //print(solution);
      //cout << "---------" << endl;
      auto new_fitness = harness.optimize(rand);
      tries[blocks[index].size()]++;
      if (fitness <= new_fitness) {

        if (fitness < new_fitness) {
          successes[blocks[index].size()]++;
          improved=true;
          donations++;
          sizes += blocks[index].size();
        }
        fitness = new_fitness;
        harness.set_check_point();
        if (fitness >= harness.max_fitness()) {
          cout << "Stop Early " << level << " " << index << endl;
          return fitness;
        }
      } else {
        harness.revert();
        /*
        if (copy != solution) {
          print(copy);
          print(solution);
          throw "SHIT";
        }
        */
      }
    }
    //cout << "No Change " << no_change << " Attempts: " << attempts << endl;
  }
  //not_solved(solution, evaluator);
  if (improved) {
    if (seen.count(solution) == 0) {
      solutions.push_back(vector<vector<bool>>(0));
      solutions.back().push_back(solution);
      seen.insert(solution);
    } else {
      //cout << "Prevented new level" << endl;
    }
    //print(solution);
  }
  return fitness;

}
