// Data structure designed to do 4 operations in O(1) for hashable types:
// 1) Insert elements only if unique
// 2) Check if element already in data structure
// 3) Remove element from data structure
// 4) Retrieve random element from data structure

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

#ifndef RANDSET_H
#define RANDSET_H

#include <unordered_map>
#include <vector>
#include <random>

// Templated on what data is being stored and what
// random number engine you want to use. Defaults to Mersenne Twister
template<class DataType, class Rand = std::mt19937>
class RandSet {
  std::unordered_map<DataType, size_t> location;
  std::vector<DataType> elements;
  size_t border = 0;
 public:
  // Add element to set IFF it is not already in set. Returns true if added. Starts On.
  bool insert(const DataType& element);
  // Returns 1 if element in set, else 0.
  size_t count(const DataType& element) {
    return location.count(element);
  }
  // Removes element from set. Returns 1 if removed, else 0.
  size_t erase(const DataType& element);
  // Returns random element from set. Undefined if set is empty.
  const DataType& random(Rand& rand);

  // Makes index returnable by "random". Returns false if already on
  bool turn_on(const size_t& element);
  // Returns true if index can be returned by "random".
  bool is_on(const size_t& element) const {
    return elements[element] < border;
  }
  // Prevents "random" from returning index. Returns false if already off
  bool turn_off(const size_t& element);
  // Turn on all elements
  void all_on() {
    border = elements.size();
  }
  // Turn off all elements
  void all_off() {
    border = 0;
  }

  // The number of elements currently in the set.
  size_t size() const {
    return border;
  }

  // The total number of elements stored.
  size_t capacity() const {
    return elements.size();
  }

  using const_iterator=decltype(elements.cbegin());
  const_iterator begin() const { return elements.begin(); }
  const_iterator end() const { return elements.begin() + border; }
};

template<class DataType, class Rand>
bool RandSet<DataType, Rand>::insert(const DataType& element) {
  auto result = location.insert( { element, elements.size() });
  // If it was actually new
  if (result.second) {
    elements.push_back(element);
    turn_on(element);
  }
  return result.second;
}

template<class DataType, class Rand>
size_t RandSet<DataType, Rand>::erase(const DataType& element) {
  turn_off(element);
  auto it = location.find(element);
  // if element in set
  if (it != location.end()) {
    // move the last element up to take "it"'s place
    DataType moving = elements.back();
    elements[it->second] = moving;
    location[moving] = it->second;
    // remove old storage
    elements.pop_back();
    location.erase(it);
    return 1;
  }
  return 0;
}

template<class DataType, class Rand>
bool RandSet<DataType, Rand>::turn_on(const size_t& element) {
  // if it is currently on
  if (location[element] < border) {
    return false;
  }
  // element which needs to stay off
  DataType moving = elements[border];
  // change "moving" and "index" in elements
  std::swap(elements[border], elements[location[element]]);
  // update the location of both
  std::swap(location[moving], location[element]);
  border++;

  return true;
}

template<class DataType, class Rand>
bool RandSet<DataType, Rand>::turn_off(const size_t& element) {
  // if it is currently off
  if (location[element] >= border) {
    return false;
  }
  border--;
  // element which needs to stay on
  DataType moving = elements[border];
  // change "moving" and "index" in elements
  std::swap(elements[border], elements[location[element]]);
  // update the location of both
  std::swap(location[moving], location[element]);

  return true;
}

template<class DataType, class Rand>
const DataType& RandSet<DataType, Rand>::random(Rand& rand) {
  return elements[std::uniform_int_distribution<size_t>(0, border - 1)(rand)];
}

#endif
