// Brian Goldman

// Collection of utilities not better suited for placement
// in other files

#include "Util.h"

// Creates a new vector containing "length" number of random bits
vector<bool> rand_vector(Random& rand, const size_t length) {
  std::uniform_int_distribution<int> rbit(0, 1);
  vector<bool> vect(length, 0);
  for (size_t index = 0; index < vect.size(); index++) {
    vect[index] = rbit(rand);
  }
  return vect;
}

// Outputs the vector to the stream as 0 and 1 characters
void print(const vector<bool> & vect, std::ostream & out) {
  for (const auto & bit : vect) {
    out << bit;
  }
  out << std::endl;
}

// Counts how many bits are different between "a" and "b".
size_t hamming_distance(const vector<bool> & a, const vector<bool> & b) {
  size_t difference = 0;
  for (size_t i = 0; i < a.size(); i++) {
    difference += a[i] != b[i];
  }
  return difference;
}
