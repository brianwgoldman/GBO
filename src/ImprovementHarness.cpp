/*
 * Neighborhood.cpp
 *
 *  Created on: Sep 10, 2014
 *      Author: goldman
 */

#include "ImprovementHarness.h"

using namespace std;

ImprovementHarness::ImprovementHarness(shared_ptr<GrayBox> evaluator_, size_t radius, Record& _recording)
  : recording(_recording) {
  recording.start_clock();
  evaluator = evaluator_;
  // Construct neighborhood
  unordered_map<size_t, unordered_set<size_t>> graph = build_graph(evaluator);
  moves_ = k_order_subgraphs(graph, radius);

  const auto& subfunctions = evaluator->epistasis();

  // Efficiency tool for coverting a bit to the subfunctions it participates in
  unordered_map<size_t, vector<size_t>> bit_to_sub;
  for (size_t sub=0; sub < subfunctions.size(); sub++) {
    for (const auto& bit: subfunctions[sub]) {
      bit_to_sub[bit].push_back(sub);
    }
  }

  sub_to_move.resize(subfunctions.size());
  move_to_sub.resize(moves_.size());
  single_bit_moves.resize(evaluator->length(), -1);

  for (size_t m=0; m < moves_.size(); m++) {
    if (moves_[m].size() == 1) {
      single_bit_moves[moves_[m][0]] = m;
    }
    for (const auto& bit: moves_[m]) {
      for (const auto& sub: bit_to_sub[bit]) {
        move_to_sub[m].insert(sub);
        sub_to_move[sub].insert(m);
      }
    }
  }

  options.resize(moves_.size());
  delta.resize(moves_.size(), 0);

  fitness=0;
  check_point_fitness=0;
  solution = nullptr;
}

int ImprovementHarness::attach(vector<bool>* solution_) {
  solution = solution_;
  fitness=0;
  // fill delta with 0s
  delta.assign(delta.size(), 0);
  for(size_t sub=0; sub < evaluator->epistasis().size(); sub++) {
    auto score = evaluator->evaluate(sub, *solution);
    fitness += score;
    for (auto move: sub_to_move[sub]) {
      delta[move] -= score;
      flip_move(move);
      delta[move] += evaluator->evaluate(sub, *solution);
      flip_move(move);
    }
  }
  options.all_on();
  recording.record(fitness);
  return fitness;
}

int ImprovementHarness::optimize(Random & rand) {
  while (options.size()) {
    auto move = options.random(rand);
    if (0 < delta[move]) {
      make_move(move); // Put in move
    }
    options.turn_off(move);
  }
  return fitness;
}

void ImprovementHarness::set_check_point() {
  saved_delta.clear();
  flipped.clear();
  check_point_fitness = fitness;
  options.all_off();
}

int ImprovementHarness::revert() {
  for (const auto& changed: saved_delta) {
    delta[changed.first] = changed.second;
  }
  for (const auto& bit: flipped) {
    (*solution)[bit] = not (*solution)[bit];
  }
  fitness = check_point_fitness;
  set_check_point();
  return fitness;
}

int ImprovementHarness::make_move(size_t move) {
  fitness += delta[move];
  recording.record(fitness);
  for (auto sub: move_to_sub[move]) {
    auto pre_move = evaluator->evaluate(sub, *solution);
    flip_move(move); // Put in move
    auto just_move = evaluator->evaluate(sub, *solution);
    flip_move(move); // Take out move
    for (auto next: sub_to_move[sub]) {
      flip_move(next); // Put in next
      auto just_next = evaluator->evaluate(sub, *solution);
      flip_move(move); // Put in move
      auto move_next = evaluator->evaluate(sub, *solution);
      flip_move(move); // Take out move
      flip_move(next); // Take out next
      if (saved_delta.count(next) == 0) {
        saved_delta[next] = delta[next];
      }
      delta[next] += (pre_move - just_next + move_next - just_move);
      options.turn_on(next);
    }
  }
  for (const auto& bit: moves_[move]) {
    if (flipped.count(bit)) {
      // two flips cancel out
      flipped.erase(bit);
    } else {
      flipped.insert(bit);
    }
  }
  flip_move(move); // Put in move
  return fitness;
}

int ImprovementHarness::modify_bit(size_t bit) {
  return make_move(single_bit_moves[bit]);
}

void ImprovementHarness::flip_move(size_t move_index) {
  for (const auto& bit: moves_[move_index]) {
    (*solution)[bit] = not (*solution)[bit];
  }
}
