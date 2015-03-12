// Brian Goldman

// Collection of utilities not better suited for placement
// in other files

#include "Util.h"

// Creates a new vector containing "length" number of random bits
void rand_vector(Random& rand, vector<bool>& solution) {
  std::uniform_int_distribution<int> rbit(0, 1);
  auto length = solution.size();
  for (size_t index = 0; index < length; index++) {
    solution[index] = rbit(rand);
  }
}

// Outputs the vector to the stream as 0 and 1 characters
void print(const vector<bool> & vect, std::ostream & out) {
  for (const auto & bit : vect) {
    out << bit;
  }
  out << std::endl;
}

// Extract bool vector from a stream of 0 and 1 characters
void read(vector<bool> & vect, std::istream & in) {
  std::string temp;
  in >> temp;

  vect.resize(temp.size());
  for (size_t i = 0; i < temp.size(); i++) {
    vect[i] = temp[i] == '1';
  }
}
