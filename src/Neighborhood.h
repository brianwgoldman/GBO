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
#include "Util.h"

// Constructs a sparse graph from the epistasis tables of the evaluator
void build_graph(shared_ptr<GrayBox> evaluator,
                 unordered_map<size_t, unordered_set<size_t>> & graph);

vector<vector<size_t>> k_order_subgraphs(
    const unordered_map<size_t, unordered_set<size_t>>& graph, size_t radius);

void recurse(const unordered_map<size_t, unordered_set<size_t>>& graph,
             size_t v, unordered_set<size_t> & closed, vector<size_t> & prev,
             unordered_set<size_t> & prevopen, size_t radius,
             vector<vector<size_t>> & found);

unordered_set<size_t> random_induced_subgraph(
    const unordered_map<size_t, unordered_set<size_t>> & graph, size_t start,
    size_t k, Random& rand);
#endif /* NEIGHBORHOOD_H_ */
