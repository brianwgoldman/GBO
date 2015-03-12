// Brian Goldman

// This is a collection of utilities used throughout programming.
// Basically just a bunch of misfits.

#ifndef UTIL_H_
#define UTIL_H_

#include <vector>
#include <iostream>
#include <random>
#include <algorithm>
#include <sstream>

using std::vector;

// Defines that the random number generator in use is the Mersenne Twister
using Random=std::mt19937;

// Generate a random bit vector of the requested length
void rand_vector(Random& rand, vector<bool>& solution);

// Print out a vector to a stream.
void print(const vector<bool> & vect, std::ostream & out = std::cout);

// Read in a vector from a stream.
void read(vector<bool> & vect, std::istream & in = std::cin);

#endif /* UTIL_H_ */
