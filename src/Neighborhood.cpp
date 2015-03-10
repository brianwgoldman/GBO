/*
 * Neighborhood.cpp
 *
 *  Created on: Oct 14, 2014
 *      Author: goldman
 */
#include "Neighborhood.h"

void build_graph(shared_ptr<GrayBox> evaluator,
                 unordered_map<size_t, unordered_set<size_t>> & graph) {
  const auto& subfunctions = evaluator->epistasis();
  for (auto subfunction : subfunctions) {
    for (auto x : subfunction) {
      for (auto y : subfunction) {
        if (x != y) {
          graph[x].insert(y);
        }
      }
    }
  }
}

vector<vector<size_t>> k_order_subgraphs(
    const unordered_map<size_t, unordered_set<size_t>>& graph, size_t radius) {
  vector<vector<size_t>> found;
  unordered_set<size_t> closed, prevopen;
  vector<size_t> prev;
  for (const auto& kv : graph) {
    closed.insert(kv.first);
    recurse(graph, kv.first, closed, prev, prevopen, radius, found);
  }
  return found;
}

void recurse(const unordered_map<size_t, unordered_set<size_t>>& graph,
             size_t v, unordered_set<size_t> & closed, vector<size_t> & prev,
             unordered_set<size_t> & prevopen, size_t radius,
             vector<vector<size_t>> & found) {
  // Create a new subset containing "prev" and "v"
  vector<size_t> inset(prev);
  inset.push_back(v);
  found.push_back(inset);
  // Add that subset to "found"
  if (inset.size() >= radius) {
    return;
  }
  vector<size_t> closed_here;
  // open everything adjacent to v
  unordered_set<size_t> openset(prevopen);
  const auto & adjacent = graph.at(v);
  openset.insert(adjacent.begin(), adjacent.end());
  // recurse using each not-closed option
  for (const auto& working : openset) {
    if (closed.count(working) == 0) {
      closed_here.push_back(working);
      closed.insert(working);
      recurse(graph, working, closed, inset, openset, radius, found);
    }
  }
  // reopen anything closed at this level
  for (const auto& working : closed_here) {
    closed.erase(working);
  }
}

unordered_set<size_t> random_induced_subgraph(
    const unordered_map<size_t, unordered_set<size_t>> & graph, size_t start,
    size_t k, Random& rand) {
  unordered_set<size_t> subset;
  vector<size_t> options(1, start);
  while (options.size() > 0 and subset.size() < k) {
    size_t index = std::uniform_int_distribution<size_t>(0, options.size() - 1)(rand);
    size_t working = options[index];
    std::swap(options[index], options.back());
    options.pop_back();
    const auto& result = subset.insert(working);
    // If it was inserted
    if (result.second) {
      const auto& adjacent = graph.at(working);
      options.insert(options.end(), adjacent.begin(), adjacent.end());
    }
  }
  return subset;
}
