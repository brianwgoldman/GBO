/*
 * Crossover.cpp
 *
 *  Created on: Sep 7, 2014
 *      Author: goldman
 */
#include "Crossover.h"
#include <unordered_set>
using namespace std;

float sfx(Random& rand, vector<bool> & first, vector<bool> & second,
         vector<bool> & result, shared_ptr<GrayBox>& evaluator, Neighborhood& neighbors) {
  // Linkage map
  unordered_map<int, vector<int>> bit_to_sub, sub_to_bit;
  for (size_t sub=0; sub < evaluator->epistasis().size(); sub ++) {
    // cout << "Sub: " << sub << endl;
    for (auto index: evaluator->epistasis()[sub]) {
      sub_to_bit[sub].push_back(index);
      bit_to_sub[index].push_back(sub);
    }
  }

  vector<bool> cross_mask(first.size(), false);
  unordered_set<int> closed;
  vector<int> priority;
  int total_crossing;
  size_t diffbits = 0;
  for (size_t i=0; i < first.size(); i++) {
    if (first[i] == second[i]) {
      cross_mask[i] = true;
    } else {
      diffbits++;
    }
  }
  if (diffbits == 0) {
    total_crossing = 0;
  } else {
    do {
      total_crossing = std::binomial_distribution<int>(diffbits, 0.5)(rand);
    } while (total_crossing == 0 or total_crossing == diffbits);
  }
  auto choice = sub_to_bit.begin();
  std::advance(choice, std::uniform_int_distribution<int>(0, sub_to_bit.size()-1)(rand));
  closed.insert(choice->first);
  int subchoice = choice->first;
  // cout << "Starting Bit: " << subchoice << " Total: " << total_crossing << endl;
  int added = 0;
  for (auto bit: choice->second) {
    cross_mask[bit] = true;
    added++;
    for (auto sub: bit_to_sub[bit]) {
      priority.push_back(sub);
    }
  }
  while (added < total_crossing) {
    while (closed.count(subchoice) and priority.size()) {
      auto index = std::uniform_int_distribution<int>(0, priority.size()-1)(rand);
      subchoice = priority[index];
      swap(priority[index], priority.back());
      priority.pop_back();
    }
    while (closed.count(subchoice)) {
      choice = sub_to_bit.begin();
      std::advance(choice, std::uniform_int_distribution<int>(0, sub_to_bit.size()-1)(rand));
      subchoice = choice->first;
    }
    // cout << "Next: " << subchoice << endl;
    closed.insert(subchoice);
    for (auto bit: sub_to_bit[subchoice]) {
      if (not cross_mask[bit]) {
        cross_mask[bit] = true;
        added++;
      }
      for (auto sub: bit_to_sub[bit]) {
        if (closed.count(sub) == 0) {
         priority.push_back(sub);
        }
      }
    }
  }
  vector<bool> o1(first.size()), o2(first.size());
  for (size_t i=0; i < first.size(); i++) {
    //cross_mask[i] = uniform_int_distribution<int>(0, 1)(rand);
    if (cross_mask[i]) {
      o1[i] = first[i];
      o2[i] = second[i];
    } else {
      o1[i] = second[i];
      o2[i] = first[i];
    }
  }
  auto f1 = hill_climb::neighbor_memory(rand, o1, evaluator, neighbors);
  auto f2 = hill_climb::neighbor_memory(rand, o2, evaluator, neighbors);

  if (f1 < f2) {
    result = o2;
    return f2;
  } else {
    result = o1;
    return f1;
  }
}

void helper(Random& rand, vector<bool> & mask, vector<size_t>& choices, unordered_set<size_t>& remaining,
            bool which, unordered_map<size_t, vector<size_t>>& bit_to_sub, unordered_map<size_t, vector<size_t>>& sub_to_bit) {
  size_t choice = mask.size() + 1;
  while (choices.size() and choice == mask.size()+1) {
    auto choice_index = std::uniform_int_distribution<size_t>(0, choices.size()-1)(rand);
    if (remaining.count(choices[choice_index])) {
      choice = choices[choice_index];
    }
    // move the last element up
    choices[choice_index] = choices.back();
    choices.pop_back();
  }
  if (choice == mask.size() + 1) {
    auto choice_it = remaining.begin();
    std::advance(choice_it, std::uniform_int_distribution<int>(0, remaining.size()-1)(rand));
    choice = *choice_it;
  }
  remaining.erase(choice);
  mask[choice] = which;
  for (auto sub: bit_to_sub[choice]) {
    for (auto bit: sub_to_bit[sub]) {
      choices.push_back(bit);
    }
  }
}

float kex(Random& rand, vector<bool> & first, vector<bool> & second,
         vector<bool> & result, shared_ptr<GrayBox>& evaluator, Neighborhood& neighbors) {
  // Linkage map
  unordered_map<size_t, vector<size_t>> bit_to_sub, sub_to_bit;
  for (size_t sub=0; sub < evaluator->epistasis().size(); sub ++) {
    // cout << "Sub: " << sub << endl;
    for (auto index: evaluator->epistasis()[sub]) {
      sub_to_bit[sub].push_back(index);
      bit_to_sub[index].push_back(sub);
    }
  }
  vector<bool> cross_mask(first.size(), false);
  unordered_set<size_t> remaining;
  for (size_t i=0; i < first.size(); i++) {
    if (first[i] != second[i]) {
      remaining.insert(i);
    }
  }
  vector<size_t> first_choose, second_choose;
  uniform_int_distribution<size_t> which(0, 1);
  while (remaining.size()) {
    auto parent = which(rand);
    if (parent) {
      helper(rand, cross_mask, first_choose, remaining, true, bit_to_sub, sub_to_bit);
    } else {
      helper(rand, cross_mask, second_choose, remaining, false, bit_to_sub, sub_to_bit);
    }
  }
  vector<bool> o1(first.size()), o2(first.size());
  for (size_t i=0; i < first.size(); i++) {
    //cross_mask[i] = uniform_int_distribution<int>(0, 1)(rand);
    if (cross_mask[i]) {
      o1[i] = first[i];
      o2[i] = second[i];
    } else {
      o1[i] = second[i];
      o2[i] = first[i];
    }
  }
  auto f1 = hill_climb::neighbor_memory(rand, o1, evaluator, neighbors);
  auto f2 = hill_climb::neighbor_memory(rand, o2, evaluator, neighbors);

  if (f1 < f2) {
    result = o2;
    return f2;
  } else {
    result = o1;
    return f1;
  }
}


float rfx(Random& rand, vector<bool> & first, vector<bool> & second,
         vector<bool> & result, shared_ptr<GrayBox>& evaluator, Neighborhood& neighbors) {
  // Linkage map
  unordered_map<size_t, vector<size_t>> bit_to_sub, sub_to_bit;
  for (size_t sub=0; sub < evaluator->epistasis().size(); sub ++) {
    // cout << "Sub: " << sub << endl;
    for (auto index: evaluator->epistasis()[sub]) {
      sub_to_bit[sub].push_back(index);
      bit_to_sub[index].push_back(sub);
    }
  }
  vector<bool> cross_mask(first.size(), false);
  unordered_set<size_t> unset;
  vector<size_t> options(evaluator->epistasis().size());
  iota(options.begin(), options.end(), 0);
  uniform_int_distribution<size_t> which(0, 1);
  for (auto sub: options) {
    bool parent = which(rand);
    for (auto bit: sub_to_bit[sub]) {
      if (unset.count(bit) == 0) {
        cross_mask[bit] = parent;
        unset.insert(bit);
      }
    }
  }
  /*
  for (size_t i=0; i < first.size(); i++) {
    if (first[i] == second[i]) {
      cout << '-';
    } else {
      cout << cross_mask[i];
    }
  }
  cout << endl;
  throw "SHIT";
  */
  vector<bool> o1(first.size()), o2(first.size());
  for (size_t i=0; i < first.size(); i++) {
    //cross_mask[i] = uniform_int_distribution<int>(0, 1)(rand);
    if (cross_mask[i]) {
      o1[i] = first[i];
      o2[i] = second[i];
    } else {
      o1[i] = second[i];
      o2[i] = first[i];
    }
  }
  auto f1 = hill_climb::neighbor_memory(rand, o1, evaluator, neighbors);
  auto f2 = hill_climb::neighbor_memory(rand, o2, evaluator, neighbors);

  if (f1 < f2) {
    result = o2;
    return f2;
  } else {
    result = o1;
    return f1;
  }
}

