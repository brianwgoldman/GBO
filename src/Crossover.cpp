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
         vector<bool> & result, shared_ptr<GrayBox>& evaluator) {
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
  do {
    total_crossing = std::binomial_distribution<int>(diffbits, 0.5)(rand);
  } while (total_crossing == 0 or total_crossing == diffbits);
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
  /*
  auto f1 = hill_climb::first_memory(rand, o1, evaluator);
  auto f2 = hill_climb::first_memory(rand, o2, evaluator);
  if (f1 < f2) {
    result = o2;
    return f2;
  } else {
    result = o1;
    return f1;
  }
  */
}


