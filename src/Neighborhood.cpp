/*
 * Neighborhood.cpp
 *
 *  Created on: Oct 14, 2014
 *      Author: goldman
 */
#include "Neighborhood.h"

unordered_map<size_t, unordered_set<size_t>> build_graph(
    shared_ptr<GrayBox> evaluator) {
  unordered_map<size_t, unordered_set<size_t>> graph;
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
  return graph;
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

