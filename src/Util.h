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

// Defines that the random number genrator in use is the Mersenne Twister
using Random=std::mt19937;

// Generate a random bit vector of the requested length
vector<bool> rand_vector(Random& rand, const size_t length);

// Print out a vector to the stream.
void print(const vector<bool> & vect, std::ostream & out = std::cout);

// Calculate the hamming distance between two bit vectors
size_t hamming_distance(const vector<bool> & a, const vector<bool> & b);

// Calculate the median.
template<class T>
T median(vector<T> data) {
  // finds the middle
  auto middle = data.size() >> 1;
  // More efficient than sorting, finds the middle element in O(N) time
  std::nth_element(data.begin(), data.begin() + middle, data.end());
  auto result = data[middle];
  // even length lists
  if (!(data.size() & 1)) {
    // Finds the middle -1 element
    std::nth_element(data.begin(), data.begin() + middle - 1, data.end());
    result = (result + data[middle - 1]) / 2;
  }
  return result;
}

#endif /* UTIL_H_ */
