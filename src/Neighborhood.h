/*
 * Neighborhood.h
 *
 *  Created on: Oct 14, 2014
 *      Author: goldman
 */

#ifndef NEIGHBORHOOD_H_
#define NEIGHBORHOOD_H_

#include "Evaluation.h"
#include <unordered_map>
using std::unordered_map;
#include <unordered_set>
using std::unordered_set;

unordered_map<size_t, unordered_set<size_t>> build_graph(shared_ptr<GrayBox> evaluator);

vector<vector<size_t>> k_order_subgraphs(const unordered_map<size_t, unordered_set<size_t>>& graph, size_t radius);

void recurse(const unordered_map<size_t, unordered_set<size_t>>& graph,
               size_t v, unordered_set<size_t> & closed, vector<size_t> & prev,
               unordered_set<size_t> & prevopen, size_t radius, vector<vector<size_t>> & found);


#endif /* NEIGHBORHOOD_H_ */
