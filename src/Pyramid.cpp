/*
 * Pyramid.cpp
 *
 *  Created on: Sep 13, 2014
 *      Author: goldman
 */

#include "Pyramid.h"

using namespace std;

Pyramid::Pyramid(Configuration& config) {
  // TODO Auto-generated constructor stub
  length = config.get<size_t>("length");
}

Pyramid::~Pyramid() {
  // TODO Auto-generated destructor stub
}

float Pyramid::iterate(Random& rand, shared_ptr<GrayBox> evaluator, Neighborhood& neighbors) {
  if (bit_to_bits.size() == 0) {
    for (size_t sub=0; sub < evaluator->epistasis().size(); sub ++) {
      for (auto index: evaluator->epistasis()[sub]) {
        //bit_to_bits[sub].insert(index);
        for (auto other: evaluator->epistasis()[sub]) {
          bit_to_bits[index].insert(other);
        }
      }
    }
    /*
    for (const auto& subbits: bit_to_bits) {
      cout << subbits.first << ": ";
      for (const auto& bit: subbits.second) {
        cout << bit << ", ";
      }
      cout << endl;
    }
    */
  }
  cout << "Prestate" << endl;
  for (size_t level=0; level < solutions.size(); level++) {
    cout << level << " " << (empty[level]?-1: fitnesses[level]) << " | ";
  }
  cout << endl;
  auto solution = rand_vector(rand, length);
  neighbors.attach(&solution);
  auto fitness = neighbors.optimize(rand);
  vector<size_t> ordering(length);
  iota(ordering.begin(), ordering.end(), 0);
  for (size_t level=0; level < solutions.size(); level++) {
    if (empty[level]) {
      solutions[level] = solution;
      fitnesses[level] = fitness;
      empty[level] = false;
      return fitness;
    } else {
      shuffle(ordering.begin(), ordering.end(), rand);
      neighbors.set_check_point();
      for (const auto& index: ordering) {
        for (size_t bit: bit_to_bits[index]) {
          //cout << bit << ", ";
          if (solution[bit] != solutions[level][bit]) {
            neighbors.modify(bit);
          }
        }
        //cout << "Modified " << neighbors.modified() << endl;
        auto new_fitness = neighbors.optimize(rand);
        //cout << "After " << neighbors.modified() << endl;
        if (fitness <= new_fitness) {
          fitness = new_fitness;
          neighbors.set_check_point();
          if (fitness >= evaluator->max_fitness()) {
            cout << "Stop Early " << level << endl;
            return fitness;
          }
        } else {
          neighbors.revert();
        }
      }
      // improved individual still worse than storage
      if (fitness <= fitnesses[level]) {
        cout << "Worse than storage" << endl;
        fitness = fitnesses[level];
        solution = solutions[level];
        neighbors.attach(&(solution));
        neighbors.set_check_point();
      }
      // current is best
      empty[level] = true;
    }
  }
  empty.push_back(false);
  fitnesses.push_back(fitness);
  solutions.push_back(solution);
  return fitness;
}

float Pyramid::generate(Random& rand, shared_ptr<GrayBox> evaluator, Neighborhood& neigbhors) {
  cout << "Prestate" << endl;
  for (size_t level=0; level < total_counts.size(); level++) {
    cout << level << " " << total_counts[level] << " | ";
  }
  cout << endl;
  auto solution = rand_vector(rand, length);
  auto fitness = hill_climb::neighbor_memory(rand, solution, evaluator, neigbhors);
  auto best = fitness;
  bool added = false;
  for (size_t level=0; level < total_counts.size(); level++) {
    if (not added) {
      for (size_t i=0; i < length; i++) {
        counts[level][i] += solution[i];
      }
      fitnesses[level] += fitness;
      total_counts[level]++;
      added = true;
    }
    if (total_counts[level] < 2) {
      break;
    }
    vector<bool> offspring(length);
    for (size_t i=0; i < length; i++) {
      offspring[i] = bernoulli_distribution(static_cast<float>(counts[level][i]) / total_counts[level])(rand);
    }
    auto new_fitness = hill_climb::neighbor_memory(rand, offspring, evaluator, neigbhors);
    if (best < new_fitness) {
      best = new_fitness;
    }
    if (fitnesses[level] / total_counts[level] < new_fitness) {
      fitness = new_fitness;
      solution = offspring;
      added = false;
    }
  }
  if (not added) {
    counts.emplace_back(solution.size(), 0);
    for (size_t i=0; i < length; i++) {
      counts.back()[i] += solution[i];
    }
    fitnesses.push_back(fitness);
    total_counts.push_back(1);
  }

  return best;
}

float Pyramid::grind(Random& rand, shared_ptr<GrayBox> evaluator, Neighborhood& neigbhors) {
  //cout << "Prestate" << endl;
  for (size_t level=0; level < solutions.size(); level++) {
    cout << level << " " << (empty[level]?-1: fitnesses[level]) << " | ";
  }
  cout << endl;
  auto solution = rand_vector(rand, length);
  auto fitness = hill_climb::neighbor_memory(rand, solution, evaluator, neigbhors);
  //return fitness;
  //cout << "Rand: " << fitness << endl;
  for (size_t level=0; level < solutions.size(); level++) {
    if (empty[level]) {
      solutions[level] = solution;
      fitnesses[level] = fitness;
      empty[level] = false;
      return fitness;
    } else {
      for (size_t repeat=0; repeat < (1<<level); repeat++) {
        vector<bool> offspring;
        auto new_fitness = rfx(rand, solutions[level], solution, offspring, evaluator, neigbhors);
        //cout << "Level: " << level << " New Fit: " << new_fitness << endl;
        if (fitness < fitnesses[level]){
          if (fitness < new_fitness) {
            solution = offspring;
            fitness = new_fitness;
          }
        } else {
          if (fitnesses[level] < new_fitness) {
            solutions[level] = offspring;
            fitnesses[level] = new_fitness;
          }
        }
        if (solutions[level] == solution) {
          break;
        }
      }

      // stored is best
      if (fitness <= fitnesses[level]) {
        fitness = fitnesses[level];
        solution = solutions[level];
      }
      // current is best
      empty[level] = true;
    }
  }
  empty.push_back(false);
  fitnesses.push_back(fitness);
  solutions.push_back(solution);
  return fitness;

}
Parallel::Parallel(Configuration& config) {
  // TODO Auto-generated constructor stub
  length = config.get<size_t>("length");
}


float Parallel::grind(Random& rand, shared_ptr<GrayBox> evaluator, Neighborhood& neigbhors) {
  cout << "Prestate" << endl;
  for (size_t level=0; level < older.size(); level++) {
    cout << level << " " << older[level].first << ", " << newer[level].first << " | ";
  }
  cout << endl;

  uniform_int_distribution<int> chooser(0, 1);

  vector<pair<float, vector<bool>>> solutions;
  solutions.emplace_back(0, vector<bool>(length, 0));
  solutions.emplace_back(0, vector<bool>(length, 0));

  auto solution = rand_vector(rand, length);
  auto fitness = hill_climb::neighbor_memory(rand, solution, evaluator, neigbhors);
  solutions.emplace_back(fitness, solution);
  solution = rand_vector(rand, length);
  fitness = hill_climb::neighbor_memory(rand, solution, evaluator, neigbhors);
  solutions.emplace_back(fitness, solution);
  float best_fit = max_element(solutions.begin(), solutions.end())->first;
  cout << "Rand: " << solutions[2].first << " " << solutions[3].first << endl;
  for (size_t level=0; level < older.size(); level++) {
    // get out the older from this level
    swap(solutions[2], older[level]);

    // advance the original newer
    swap(older[level], newer[level]);
    if (solutions[2].second == solutions[3].second) {
      cout << "Equal skip" << endl;
      continue;
    }
    // uniform crossover
    for (size_t bit=0; bit < length; bit++) {
      if (chooser(rand)) {
        solutions[0].second[bit] = solutions[2].second[bit];
        solutions[1].second[bit] = solutions[3].second[bit];
      } else {
        solutions[1].second[bit] = solutions[2].second[bit];
        solutions[0].second[bit] = solutions[3].second[bit];
      }
    }
    // hill climb evaluate
    solutions[0].first = hill_climb::neighbor_memory(rand, solutions[0].second, evaluator, neigbhors);
    solutions[1].first = hill_climb::neighbor_memory(rand, solutions[1].second, evaluator, neigbhors);
    cout << "Level: " << level << " Fit: " << solutions[0].first << " " << solutions[1].first << endl;
    sort(solutions.begin(), solutions.end());
    best_fit = solutions[3].first;
  }
  older.push_back(solutions[3]);
  newer.push_back(solutions[2]);
  return best_fit;
}

Sharing::Sharing(Configuration& config) {
  // TODO Auto-generated constructor stub
  length = config.get<size_t>("length");
}


float Sharing::grind(Random& rand, shared_ptr<GrayBox> evaluator, Neighborhood& neigbhors) {
  /*
  cout << "Prestate" << endl;
  for (size_t level=0; level < solutions.size(); level++) {
    cout << level << " " << solutions[level].first << " | ";
  }
  cout << endl;
  //*/

  auto solution = rand_vector(rand, length);
  auto fitness = hill_climb::neighbor_memory(rand, solution, evaluator, neigbhors);
  solutions.emplace_back(fitness, solution);
  size_t mask = 1;
  cout << "Size: " << solutions.size() << " = ";
  vector<pair<float, vector<bool>>> cross_solutions;
  uniform_int_distribution<int> chooser(0, 1);
  cross_solutions.emplace_back(0, vector<bool>(length, 0));
  cross_solutions.emplace_back(0, vector<bool>(length, 0));
  cross_solutions.emplace_back(0, vector<bool>(length, 0));
  cross_solutions.emplace_back(0, vector<bool>(length, 0));
  while ((solutions.size() & mask) == 0) {
    cout << mask << ", ";
    for (size_t i=0; i < mask; i++) {
      size_t p1 = solutions.size()-i-1;
      size_t p2 = solutions.size()-i-mask-1;
      //cout << "(" << p1 << ", " << p2 << ") ";
      swap(solutions[p1], cross_solutions[2]);
      swap(solutions[p2], cross_solutions[3]);
      // create two solutions via crossover
      for (size_t bit=0; bit < length; bit++) {
        if (chooser(rand)) {
          cross_solutions[0].second[bit] = cross_solutions[2].second[bit];
          cross_solutions[1].second[bit] = cross_solutions[3].second[bit];
        } else {
          cross_solutions[1].second[bit] = cross_solutions[2].second[bit];
          cross_solutions[0].second[bit] = cross_solutions[3].second[bit];
        }
      }
      // hill climb evaluate
      cross_solutions[2].first = hill_climb::neighbor_memory(rand, cross_solutions[2].second, evaluator, neigbhors);
      cross_solutions[3].first = hill_climb::neighbor_memory(rand, cross_solutions[3].second, evaluator, neigbhors);
      sort(cross_solutions.begin(), cross_solutions.end());
      swap(solutions[p1], cross_solutions[2]);
      swap(solutions[p2], cross_solutions[3]);
      if (fitness < solutions[p2].first) {
        fitness = solutions[p2].first;
      }
    }
    mask <<= 1;
  }
  cout << endl;
  return fitness;
}

float get_fit(vector<bool>& solution, shared_ptr<GrayBox> evaluator) {
  float total = 0;
  for (size_t i=0; i < evaluator->epistasis().size(); i++) {
    total += evaluator->evaluate(i, solution);
  }
  return total;
}

void not_solved(vector<bool>& solution, shared_ptr<GrayBox> evaluator) {
  cout << "Not Solved: ";
  for (size_t i=0; i < evaluator->epistasis().size(); i++) {
    if(evaluator->evaluate(i, solution) != 1) {
      cout << i << ", ";
    }
  }
  cout << endl;
}

P3like::P3like(Configuration& config): length(config.get<size_t>("length")),
    hc(config.get<hill_climb::pointer>("hill_climber")) {
  ordering.resize(length);
  iota(ordering.begin(), ordering.end(), 0);
  option = config.get<string>("option");
  successes.resize(length, 0);
  tries.resize(length, 0);
}

void sfx_tree(Random& rand, shared_ptr<GrayBox> evaluator, vector<vector<size_t>> & blocks) {
  for (size_t i=0; i < evaluator->length(); i++) {
    // creates vectors of size 1 with value i
    blocks.emplace_back(1, i);
  }
  vector<size_t> options(evaluator->epistasis().size());
  iota(options.begin(), options.end(), 0);
  vector<size_t> bit_to_block(evaluator->length());
  iota(bit_to_block.begin(), bit_to_block.end(), 0);
  shuffle(options.begin(), options.end(), rand);
  // for each subfunction in a random order
  for (const auto& sub: options) {
    unordered_set<size_t> block_numbers;
    // for each bit in the subfunction
    for (const auto& bit: evaluator->epistasis()[sub]) {
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
      //if (blocks.back().size() >= evaluator->length()/2) {
      if (false and largest >= evaluator->length()/2) {
        blocks.pop_back();
      } else {
        // assign moved bits to new block
        for (const auto& bit: blocks.back()) {
          bit_to_block[bit] = blocks.size() - 1;
        }
      }
    }
  }

  if (blocks.back().size() == evaluator->length()) {
    blocks.pop_back();
  }
  blocks.erase(blocks.begin(), blocks.begin()+evaluator->length());
}

void loose_tree(Random& rand, shared_ptr<GrayBox> evaluator, vector<vector<size_t>> & blocks) {
  for (size_t i=0; i < evaluator->length(); i++) {
    // creates vectors of size 1 with value i
    blocks.emplace_back(1, i);
  }
  vector<size_t> bit_to_block(evaluator->length());
  iota(bit_to_block.begin(), bit_to_block.end(), 0);

  vector<vector<size_t>> relations(evaluator->length());
  for (const auto& sub: evaluator->epistasis()) {
    for (const auto& b1: sub) {
      for (const auto& b2: sub) {
        relations[b1].push_back(b2);
      }
    }
  }
  vector<size_t> options(relations.size());
  iota(options.begin(), options.end(), 0);
  while (options.size() > 0) {
    shuffle(options.begin(), options.end(), rand);
    // for each subfunction in a random order
    for (size_t i=0; i < options.size(); i++) {
      size_t b1 = options[i];
      unordered_set<size_t> block_numbers;
      // for each bit in the subfunction
      block_numbers.insert(bit_to_block[b1]);
      if (relations[b1].size() > 0) {
        size_t choice = uniform_int_distribution<size_t>(0, relations[b1].size() - 1)(rand);
        block_numbers.insert(bit_to_block[relations[b1][choice]]);
        swap(relations[b1][choice], relations[b1].back());
        relations[b1].pop_back();
      } else {
        swap(options[i], options.back());
        options.pop_back();
        i--;
        continue;
      }

      // a merge is necessary
      if (block_numbers.size() > 1) {
        // start a new empty block
        blocks.push_back(vector<size_t>(0));
        // the same bit cannot appear in two blocks, so just combine
        for (const auto& block_number: block_numbers) {
          for (const auto& bit: blocks[block_number]) {
            blocks.back().push_back(bit);
          }
        }
        if (false and blocks.back().size() >= evaluator->length()/2) {
          blocks.pop_back();
        } else {
          // assign moved bits to new block
          for (const auto& bit: blocks.back()) {
            bit_to_block[bit] = blocks.size() - 1;
          }
        }
      }
    }
  }
  blocks.erase(blocks.begin(), blocks.begin()+evaluator->length());
}

void easy_tree(Random& rand, shared_ptr<GrayBox> evaluator, vector<vector<size_t>> & blocks) {
  unordered_map<size_t, unordered_set<size_t>> bit_to_bits;
  for (size_t sub=0; sub < evaluator->epistasis().size(); sub ++) {
    for (auto index: evaluator->epistasis()[sub]) {
      for (auto other: evaluator->epistasis()[sub]) {
        bit_to_bits[index].insert(other);
      }
    }
  }
  for (const auto& mask: bit_to_bits) {
    blocks.emplace_back(mask.second.begin(), mask.second.end());
  }
}

void sub_tree(Random& rand, shared_ptr<GrayBox> evaluator, vector<vector<size_t>> & blocks) {
  for (const auto& sub: evaluator->epistasis()) {
    blocks.emplace_back(sub.begin(), sub.end());
  }
}

void sfx_graph(Random& rand, shared_ptr<GrayBox> evaluator, vector<vector<size_t>> & blocks) {
  unordered_map<size_t, unordered_set<size_t>> bit_to_sub;
  for (size_t sub=0; sub < evaluator->epistasis().size(); sub ++) {
    for (auto index: evaluator->epistasis()[sub]) {
      bit_to_sub[index].insert(sub);
    }
  }
  for (size_t bit=0; bit < evaluator->length(); bit++) {
    vector<size_t> subs(bit_to_sub[bit].begin(), bit_to_sub[bit].end());
    unordered_set<size_t> block;
    block.insert(bit);
    for (size_t loop=0; loop < 5 and subs.size() > 0; loop++) {
      size_t choice = uniform_int_distribution<size_t>(0, subs.size()-1)(rand);
      size_t sub = subs[choice];
      swap(subs[choice], subs.back());
      subs.pop_back();
      bool added = false;
      for (const auto& index: evaluator->epistasis()[sub]) {
        if (block.count(index) == 0) {
          added = true;
          block.insert(index);
          for (const auto& new_sub: bit_to_sub[index]) {
            subs.push_back(new_sub);
          }
        }
      }
      if (added) {
        blocks.emplace_back(block.begin(), block.end());
      }
    }
  }
}

float P3like::grind(Random& rand, shared_ptr<GrayBox> evaluator, Neighborhood& neighbors) {
  if (bit_to_bits.size() == 0) {
    for (size_t sub=0; sub < evaluator->epistasis().size(); sub ++) {
      for (auto index: evaluator->epistasis()[sub]) {
        for (auto other: evaluator->epistasis()[sub]) {
          bit_to_bits[index].insert(other);
        }
      }
    }
  }
  /*
  for (size_t level=0; level < solutions.size(); level++) {
    cout << level << " " << solutions[level].size() << " | ";
  }
  cout << endl;
  */
  auto solution = rand_vector(rand, length);
  neighbors.attach(&solution);
  //auto fitness = hc(rand, solution, evaluator, neighbors);
  auto fitness = neighbors.optimize(rand);
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
    if (option == "sub") {
      sub_tree(rand, evaluator, blocks);
    } else if (option == "sfx") {
      sfx_tree(rand, evaluator, blocks);
    } else if (option == "easy") {
      easy_tree(rand, evaluator, blocks);
    } else if (option == "loose") {
      loose_tree(rand, evaluator, blocks);
    } else if (option == "graph") {
      sfx_graph(rand, evaluator, blocks);
    } else {
      cout << "BAD OPTION" << endl;
      throw "SHIT";
    }
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
    //cout << "Average block size: " << static_cast<float>(sum) / blocks.size() << " total: " << blocks.size() << endl;
    /*
    for (size_t i=0; i < length; i++) {
      cout << i << ": " << freq[i] << endl;
    }
    throw "SHIT";
    //*/

    // uniform_int_distribution<size_t> rand_donor(0, solutions[level].size()-1);
    vector<size_t> options(solutions[level].size());
    iota(options.begin(), options.end(), 0);
    shuffle(ordering.begin(), ordering.end(), rand);
    size_t no_change=0, attempts=0;
    //for (size_t index=0; index < solution.size(); index++) {
    for (size_t index=0; index < blocks.size(); index++) {
      //auto copy = solution;
      size_t limit = options.size();
      neighbors.set_check_point();
      while (limit > 0 and neighbors.modified() == 0) {
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
            neighbors.modify(bit);
          }
        }
      }
      if (neighbors.modified() == 0) {
        no_change++;
      }
      //cout << endl;
      //print(solution);
      //cout << "---------" << endl;
      auto new_fitness = neighbors.optimize(rand);
      tries[blocks[index].size()]++;
      if (fitness <= new_fitness) {

        if (fitness < new_fitness) {
          successes[blocks[index].size()]++;
          improved=true;
          donations++;
          sizes += blocks[index].size();
        }
        fitness = new_fitness;
        neighbors.set_check_point();
        if (fitness >= evaluator->max_fitness()) {
          cout << "Stop Early " << level << " " << index << endl;
          return fitness;
        }
      } else {
        neighbors.revert();
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
  evaluator->reweight(solution);
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
  /*
  if (donations > 0) {
    cout << "Average size of successful donation: " << static_cast<float>(sizes) / donations << " total: " << donations << endl;
  }

  vector<size_t> magic;
  for (size_t i=0; i < length; i++) {
    if (successes[i] > 100) {
      magic.push_back(i);
    }
  }
  sort(magic.begin(), magic.end(), [this](size_t x, size_t y){return static_cast<float>(this->successes[x]) / this->tries[x] > static_cast<float>(this->successes[y]) / this->tries[y];});
  for (size_t i=0; i < 10 and i < magic.size(); i++) {
    cout << "(" << magic[i] << ": " << successes[magic[i]] << "/" << tries[magic[i]] << "), ";
  }
  cout << endl;
  */
  /*
  auto real_fit = get_fit(solution, evaluator);
  if (real_fit != fitness) {
    cout << "Bad Fitness" << endl;
    throw "SHIT";
  }
  */
  return fitness;

}


float Lambda::grind(Random& rand, shared_ptr<GrayBox> evaluator, Neighborhood& neighbors) {
  cout << "Lambda: " << lambda << " seen: " << seen.size() << endl;
  if (solution.size() != length) {
    solution = rand_vector(rand, length);
    neighbors.attach(&solution);
    fitness = neighbors.optimize(rand);
    seen.insert(solution);
  }

  neighbors.set_check_point();
  uniform_int_distribution<size_t> rand_bit(0, length-1);
  // modify random bits
  for (size_t i=0; i < lambda; i++) {
    neighbors.modify(rand_bit(rand));
  }
  auto new_fitness = neighbors.optimize(rand);
  // reached a new local optima
  /*
  if (seen.count(solution) == 0) {
    seen.insert(solution);
    if (lambda > 1) {
      lambda--;
    }
  } else {
    lambda++;
  }
  */
  if (new_fitness <= fitness) {
    lambda*=2;
  } else {
    lambda=1;
  }
  if (new_fitness < fitness) {
    neighbors.revert();

  } else {


    //cout << "Moved" << neighbors.modified() << endl;
    neighbors.set_check_point();
    fitness = new_fitness;
  }
  if (lambda >= length) {
    solution.clear();
    lambda=1;
  }
  return new_fitness;
}

float DownUp::grind(Random& rand, shared_ptr<GrayBox> evaluator, Neighborhood& neighbors) {
  if (solution.size() != length) {
    solution = rand_vector(rand, length);
    neighbors.attach(&solution);
    fitness = neighbors.optimize(rand);
  }
  neighbors.reset();
  neighbors.make_bad(rand);
  neighbors.reset();
  return neighbors.optimize(rand);

}
