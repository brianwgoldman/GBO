// Data structure designed to efficiently choose at random from a subset of indices.
// The following 4 operations require O(1) time
// 1) Add an index to the set (turn_on)
// 2) Check if index is in the set (is_on)
// 3) Remove index from the set (turn_off)
// 4) Retrieve random index from the set (random)
// 5) Change the range of indices (resize)

// Copyright (c) 2014, Brian W. Goldman
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// The views and conclusions contained in the software and documentation are those
// of the authors and should not be interpreted as representing official policies,
// either expressed or implied, of the FreeBSD Project.

#ifndef RANDINDEX_HPP
#define RANDINDEX_HPP

#include <vector>
#include <random>

// Templated on random number engine you want to use. Defaults to Mersenne Twister
template<class Rand = std::mt19937>
class RandIndex {
  // stores all indices
  std::vector<size_t> elements;
  // keeps track of where each index appears in elements
  std::vector<size_t> location;
  // separates indices which are in the set from not in the set
  size_t border;
 public:
  // Creates the set containing an index for [0,length-1]. All indices start on.
  RandIndex(const size_t& length = 0);
  // Makes index returnable by "random". Returns false if already on
  bool turn_on(const size_t& index);
  // Returns true if index can be returned by "random".
  bool is_on(const size_t& index) const {
    return location[index] < border;
  }
  // Prevents "random" from returning index. Returns false if already off
  bool turn_off(const size_t& index);
  // Returns random index from set. Undefined if set is empty.
  const size_t& random(Rand& rand) const;
  // Change the maximum index allowable. All newly added indices start on.
  void resize(const size_t& length);
  const size_t& size() const {
    return border;
  }
  void all_on() {
    border = elements.size();
  }
  void all_off() {
    border = 0;
  }
};

template<class Rand>
RandIndex<Rand>::RandIndex(const size_t& length) {
  location.resize(length);
  elements.resize(length);
  border = length;
  iota(location.begin(), location.end(), 0);
  iota(elements.begin(), elements.end(), 0);
}

template<class Rand>
bool RandIndex<Rand>::turn_on(const size_t& index) {
  // if it is currently on
  if (location[index] < border) {
    return false;
  }
  // element which needs to stay off
  size_t moving = elements[border];
  // change "moving" and "index" in elements
  std::swap(elements[border], elements[location[index]]);
  // update the location of both
  std::swap(location[moving], location[index]);
  border++;

  return true;
}

template<class Rand>
bool RandIndex<Rand>::turn_off(const size_t& index) {
  // if it is currently off
  if (location[index] >= border) {
    return false;
  }
  border--;
  // element which needs to stay on
  size_t moving = elements[border];
  // change "moving" and "index" in elements
  std::swap(elements[border], elements[location[index]]);
  // update the location of both
  std::swap(location[moving], location[index]);

  return true;
}

template<class Rand>
const size_t& RandIndex<Rand>::random(Rand& rand) const {
  return elements[std::uniform_int_distribution<size_t>(0, border - 1)(rand)];
}

template<class Rand>
void RandIndex<Rand>::resize(const size_t& length) {
  size_t previous_size = elements.size();
  // need to expand
  if (previous_size < length) {
    location.resize(length);
    elements.resize(length);
    // fill in new data
    iota(location.begin() + previous_size, location.end(), previous_size);
    iota(elements.begin() + previous_size, elements.end(), previous_size);
    // turn all of the new stuff on
    for (size_t index = previous_size; index < length; index++) {
      turn_on(index);
    }
    // need to contract
  } else if (previous_size > length) {
    // for each thing that needs to be removed
    for (size_t index = length; index < elements.size(); index++) {
      // make certain it is not in the set
      turn_off(index);
      // overwrite index with whatever is about to be lost
      previous_size--;
      size_t moving = elements[previous_size];
      elements[location[index]] = moving;
      location[moving] = location[index];
    }
    location.resize(length);
    elements.resize(length);
  }
}
#endif
