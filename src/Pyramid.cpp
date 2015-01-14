/*
 * Pyramid.cpp
 *
 *  Created on: Sep 13, 2014
 *      Author: goldman
 */

#include "Pyramid.h"

using namespace std;

Pyramid::Pyramid(Random& _rand, Configuration& _config,
                 ImprovementHarness& _harness)
    : Optimizer(_rand, _config, _harness), sfx_options(harness.epistasis().size()) {
  iota(sfx_options.begin(), sfx_options.end(), 0);
}

// TODO Idea about using subfunctions to come up with random colorings of the graph
void Pyramid::sfx_tree(vector<vector<size_t>> & blocks) {
  for (size_t i = 0; i < length; i++) {
    // creates vectors of size 1 with value i
    blocks.emplace_back(1, i);
  }

  vector<size_t> bit_to_block(length);
  iota(bit_to_block.begin(), bit_to_block.end(), 0);

  shuffle(sfx_options.begin(), sfx_options.end(), rand);
  // for each subfunction in a random order
  for (const auto& sub : sfx_options) {
    unordered_set<size_t> block_numbers;
    // for each bit in the subfunction
    for (const auto& bit : harness.epistasis()[sub]) {
      block_numbers.insert(bit_to_block[bit]);
    }
    // a merge is necessary
    if (block_numbers.size() > 1) {
      // start a new empty block
      blocks.push_back(vector<size_t>(0));
      // the same bit cannot appear in two blocks, so just combine
      for (const auto& block_number : block_numbers) {
        for (const auto& bit : blocks[block_number]) {
          blocks.back().push_back(bit);
        }
      }
      // assign moved bits to new block
      for (const auto& bit : blocks.back()) {
        bit_to_block[bit] = blocks.size() - 1;
      }
    }
  }

  if (blocks.back().size() == length) {
    blocks.pop_back();
  }
  blocks.erase(blocks.begin(), blocks.begin() + length);
}

void Pyramid::alt_tree(vector<vector<size_t>> & blocks) {
  size_t usable = sfx_options.size();
  vector<size_t> sizes(2);
  sizes[0] = uniform_int_distribution<size_t>(1, length - 1)(rand);
  sizes[1] = length - sizes[0];

  for (size_t sizes_index = 0; sizes_index < sizes.size(); sizes_index++) {
    size_t working_size = sizes[sizes_index];
    if (working_size > 1) {
      sizes.push_back(uniform_int_distribution<size_t>(1, working_size - 1)(rand));
      sizes.push_back(working_size - sizes.back());
    }

    unordered_set<size_t> block;
    while (block.size() < working_size) {
      // Get a random subfunction
      usable--;
      size_t sub_index = uniform_int_distribution<size_t>(0, usable)(rand);
      const auto& sub = harness.epistasis()[sfx_options[sub_index]];
      swap(sfx_options[sub_index], sfx_options[usable]);
      block.insert(sub.begin(), sub.end());
      if (usable == 0) {
        usable = length;
      }
    }
    blocks.emplace_back(block.begin(), block.end());
  }
}

void Pyramid::add_if_unique(const vector<bool>& candidate, size_t level) {
  if (seen.count(candidate) == 0) {
    if (solutions.size() == level) {
      solutions.push_back(vector<vector<bool>>(0));
      selector_tool.push_back(vector<size_t>(0));
    }

    selector_tool[level].push_back(selector_tool[level].size());
    solutions[level].push_back(candidate);
    seen.insert(candidate);
  }
}

int Pyramid::iterate() {
  rand_vector(rand, solution);
  harness.attach(&solution);
  auto fitness = harness.optimize(rand);

  bool improved = true;

  for (size_t level = 0; level < solutions.size(); level++) {
    if (improved) {
      add_if_unique(solution, level);
      improved = false;
    }
    vector<vector<size_t>> blocks;
    alt_tree(blocks);
    shuffle(blocks.begin(), blocks.end(), rand);

    auto& options = selector_tool[level];
    //size_t limiter = min(blocks.size(), solutions[level].size());
    size_t limiter = blocks.size();
    for (size_t index = 0; index < limiter; index++) {
      size_t limit = options.size();
      harness.set_check_point();
      while (limit > 0 and harness.modified() == 0) {

        size_t choice = uniform_int_distribution<size_t>(0, limit - 1)(rand);
        size_t donor = options[choice];
        swap(options[choice], options[limit - 1]);
        limit--;

        for (size_t bit : blocks[index]) {
          if (solution[bit] != solutions[level][donor][bit]) {
            harness.modify_bit(bit);
          }
        }
      }

      auto new_fitness = harness.optimize(rand);

      if (fitness <= new_fitness) {
        if (fitness < new_fitness) {
          improved = true;
        }
        fitness = new_fitness;
        harness.set_check_point();
        if (fitness >= harness.max_fitness()) {
          return fitness;
        }
      } else {
        harness.revert();
      }
    }
  }
  if (improved) {
    add_if_unique(solution, solutions.size());
  }
  return fitness;
}
