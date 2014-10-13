/*
 * Crossover.h
 *
 *  Created on: Sep 7, 2014
 *      Author: goldman
 */

#ifndef CROSSOVER_H_
#define CROSSOVER_H_
#include "Util.h"
#include "Evaluation.h"
#include "HillClimb.h"

float sfx(Random& rand, vector<bool> & first, vector<bool> & second,
         vector<bool> & result, shared_ptr<GrayBox>& evaluator, Neighborhood& neighbors);

float kex(Random& rand, vector<bool> & first, vector<bool> & second,
         vector<bool> & result, shared_ptr<GrayBox>& evaluator, Neighborhood& neighbors);

float rfx(Random& rand, vector<bool> & first, vector<bool> & second,
         vector<bool> & result, shared_ptr<GrayBox>& evaluator, Neighborhood& neighbors);




#endif /* CROSSOVER_H_ */
