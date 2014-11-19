// Brian Goldman

// Each evaluation object defines the "evaluate" function for how
// to convert the vector of bool solution into a floating point fitness.

#include "Evaluation.h"
using namespace std;

// Template specialization to convert a string to a function pointer
// Used to allow configurable problem choice
template<>
evaluation::pointer Configuration::get(const string key) const {
  return evaluation::lookup[get<string>(key)];
}
DeceptiveTrap::DeceptiveTrap(Configuration& config, int run_number)
      : trap_size(config.get<int>("trap_size")) {
  length_ = config.get<int>("length");
  for (size_t i=0; i < length_; i+= trap_size) {
    epistasis_.emplace_back(trap_size, 0);
    iota(epistasis_.back().begin(), epistasis_.back().end(), i);
  }
}

// Iterate over traps, sum partial scores
int DeceptiveTrap::evaluate(size_t subfunction, const vector<bool> & solution) {
  size_t partial = 0;
  for (auto index: epistasis_[subfunction]) {
    partial += solution[index];
  }

  // Make it deceptive
  if (partial < trap_size) {
    partial = trap_size - partial - 1;
  }

  // Convert to percentage of total
  return partial;
}

// Attempts to load the problem file, otherwise constructs a new problem
// solves it, and saves it to a problem file
NearestNeighborNK::NearestNeighborNK(Configuration& config, int run_number) {
  k = config.get<int>("k");
  length_ = config.get<int>("length");
  table.resize(length_, vector<size_t>(2 << k, 0));
  int rng_seed = config.get<int>("problem_seed") + run_number;
  cout << "Problem Seed: " << rng_seed << " " << config.get<int>("problem_seed") << endl;
  for (size_t i=0; i < length_; i++) {
    epistasis_.push_back(vector<size_t>());
    for (size_t x=0; x <= k; x++) {
      epistasis_.back().push_back((i+x)%length_);
    }
  }
  // Build up the filename where this problem is stored
  string filename = config.get<string>("problem_folder");
  filename += +"NearestNeighborNK_";
  filename += config.get<string>("length") + "_";
  filename += config.get<string>("k") + "_";
  filename += to_string(rng_seed) + ".txt";
  ifstream in(filename);
  // If this problem has been tried before
  if (in) {
    // Read in information about the global minimum
    in >> minimum;
    worst.resize(length_);
    string temp;
    in >> temp;
    for (size_t i = 0; i < length_; i++) {
      worst[i] = temp[i] == '1';
    }

    // Read in information about the global maximum
    in >> maximum;
    best.resize(length_);
    in >> temp;
    for (size_t i = 0; i < length_; i++) {
      best[i] = temp[i] == '1';
    }

    // Read in the fitness table
    for (auto& row : table) {
      for (auto& entry : row) {
        in >> entry;
      }
    }
    in.close();
  } else {
    // Generate the new problem
    Random rand(rng_seed);

    // Generate the table
    auto generator = std::uniform_int_distribution<size_t>(0, 2 << k);
    for (auto& row : table) {
      for (auto& entry : row) {
        entry = generator(rand);
      }
    }

    // Find its minimum and maximum
    minimum = solve(worst, false);
    maximum = solve(best, true);

    // Write it out to the file
    ofstream out(filename);
    out << minimum << " ";
    print(worst, out);

    out << maximum << " ";
    print(best, out);

    for (auto& row : table) {
      for (auto& entry : row) {
        out << entry << " ";
      }
      out << endl;
    }
    out.close();
  }
}

// Used in finding the minimum / maximum of the generated problem.
//
size_t NearestNeighborNK::chunk_fitness(trimap& known, size_t chunk_index,
                                       size_t a, size_t b) {
  // If we know the fitness, return it
  const auto& first = known.find(chunk_index);
  if (first != known.end()) {
    const auto& second = first->second.find(a);
    if (second != first->second.end()) {
      const auto& third = second->second.find(b);
      if (third != second->second.end()) {
        return third->second;
      }
    }
  }

  // Calculate the fitness
  size_t fitness = 0;
  size_t mask = (2 << k) - 1;

  size_t combined = (a << k) | b;
  combined = (combined << 1) | (a & 1);

  for (size_t g = 0; g < k; g++) {
    size_t value = (combined >> (k - g)) & mask;
    fitness += table[chunk_index * k + g][value];
  }

  // Record the chuck fitness for later
  known[chunk_index][a][b] = fitness;
  return fitness;
}

// Converts a number into a series of bits
void NearestNeighborNK::int_into_bit(size_t src, vector<bool>& dest) {
  for (size_t i = 1; i <= k; i++) {
    dest.push_back((src >> (k - i)) & 1);
  }
}

// Find either the minimum or maximum (depending on the last argument) of the problem
// See the following paper for full explanation:
// "The computational complexity of N-K fitness functions"
// by A. H. Wright, R. K. Thompson, and J. Zhang
size_t NearestNeighborNK::solve(vector<bool>& solution, bool maximize) {
  size_t numbers = 1 << k;
  trimap known;
  std::unordered_map<size_t,
      std::unordered_map<size_t, std::unordered_map<size_t, size_t> > > partial;
  size_t current;
  // Iteratively build up partial values and their fitnesses
  for (size_t n = length_ / k - 1; n > 1; n--) {
    // stores the fitness of settings
    std::unordered_map<size_t, std::unordered_map<size_t, size_t> > utility;
    // stores the bit values that achieve best partial fitnesses
    std::unordered_map<size_t, std::unordered_map<size_t, size_t> > value;
    // Loop through the pieces
    for (size_t left = 0; left < numbers; left++) {
      for (size_t right = 0; right < numbers; right++) {
        utility[left][right] = 0;
        if (not maximize) {
          utility[left][right] = (2 << k) * length_;
        }
        for (size_t middle = 0; middle < numbers; middle++) {
          current = chunk_fitness(known, n - 1, left, middle);
          current += chunk_fitness(known, n, middle, right);
          if ((maximize and utility[left][right] <= current)
              or (not maximize and utility[left][right] >= current)) {
            utility[left][right] = current;
            value[left][right] = middle;
          }
        }
      }
    }

    // Extract information
    for (size_t left = 0; left < numbers; left++) {
      for (size_t right = 0; right < numbers; right++) {
        known[n - 1][left][right] = utility[left][right];
        partial[n][left][right] = value[left][right];
      }
    }
  }

  // Initialize fitness to be worse than worst possible
  size_t fitness = 0;
  if (not maximize) {
    fitness = (2 << k) * length_;
  }

  // Find the best left and right values
  size_t best_left = 0;
  size_t best_right = 0;
  for (size_t left = 0; left < numbers; left++) {
    for (size_t right = 0; right < numbers; right++) {
      current = chunk_fitness(known, 0, left, right);
      current += chunk_fitness(known, 1, right, left);
      if ((maximize and fitness <= current)
          or (not maximize and fitness >= current)) {
        fitness = current;
        best_left = left;
        best_right = right;
      }
    }
  }

  // Recreate the optimal string
  solution.clear();
  solution.reserve(length_);
  int_into_bit(best_left, solution);
  int_into_bit(best_right, solution);
  size_t last = best_right;
  for (size_t i = 2; i < length_ / k; i++) {
    last = partial[i][last][best_left];
    int_into_bit(last, solution);
  }
  return fitness;
}

// Use the table to evaluate the quality of the solution
int NearestNeighborNK::evaluate(size_t subfunction, const vector<bool> & solution) {
  // Construct the integer represented by this subset of the solution
  size_t index = 0;
  //for (size_t neighbor = subfunction; neighbor <= i + k; neighbor++) {
  for (const auto& neighbor: epistasis_[subfunction]) {
    index = (index << 1) | (solution[neighbor]^best[neighbor]);
  }

  // Convert to percentage of total
  return table[subfunction][index];
}

UnrestrictedNK::UnrestrictedNK(Configuration& config, int run_number) {
  k = config.get<int>("k");
  length_ = config.get<int>("length");
  table.resize(length_, vector<size_t>(2 << k, 0));
  int rng_seed = config.get<int>("problem_seed") + run_number;

  vector<size_t> options(length_);
  iota(options.begin(), options.end(), 0);
  Random rand(rng_seed);
  for (size_t i=0; i < length_; i++) {
    shuffle(options.begin(), options.end(), rand);
    epistasis_.push_back(vector<size_t>(1, i));
    //cout << epistasis_.back().back() << ", ";
    for (size_t x=0; x < k; x++) {
      epistasis_.back().push_back(options[x]);
      //cout << epistasis_.back().back() << ", ";
    }
    //cout << endl;
  }

  // Generate the table
  auto generator = std::uniform_int_distribution<size_t>(0, 2 << k);
  maximum = 0;
  for (auto& row : table) {
    size_t best = 0;
    for (auto& entry : row) {
      entry = generator(rand);
      if (best < entry) {
        best = entry;
      }
    }
    maximum += best;
  }
}


// Use the table to evaluate the quality of the solution
int UnrestrictedNK::evaluate(size_t subfunction, const vector<bool> & solution) {
  // Construct the integer represented by this subset of the solution
  size_t index = 0;
  //for (size_t neighbor = subfunction; neighbor <= i + k; neighbor++) {
  for (const auto& neighbor: epistasis_[subfunction]) {
    index = (index << 1) | (solution[neighbor]);
  }

  // Convert to percentage of total
  return table[subfunction][index];
}


// Generates the new problem each time its needed, based on
// the problem see and run number
MAXSAT::MAXSAT(Configuration& config, int run_number) {
  length_ = config.get<int>("length");
  epistasis_.resize(config.get<float>("clause_ratio") * length_, vector<size_t>(3));
  signs.resize(epistasis_.size());
  weights.resize(epistasis_.size(), 1);
  total_weights = epistasis_.size();
  int rng_seed = config.get<int>("problem_seed") + run_number;
  Random rand(rng_seed);

  // Create the random target solution
  vector<bool> solution = rand_vector(rand, length_);

  // Data structure used to select random variables to include in a clause
  vector<int> options(length_);
  std::iota(options.begin(), options.end(), 0);

  // Distributions for selecting 3 variables at random
  std::uniform_int_distribution<> dist[] = {
      std::uniform_int_distribution<>(0, length_ - 1),
      std::uniform_int_distribution<>(1, length_ - 1),
      std::uniform_int_distribution<>(2, length_ - 1)
  };

  auto sign_select = std::uniform_int_distribution<>(0, sign_options.size() - 1);

  for (size_t i = 0; i < epistasis_.size(); i++) {
    // Choose a random sign setting for this clause
    int select = sign_select(rand);

    // For each term in this clause
    for (int c = 0; c < 3; c++) {
      // Choose a random variable to be a term in the clause
      std::swap(options[c], options[dist[c](rand)]);
      epistasis_[i][c] = options[c];
      // Set the sign to agree / disagree with the solution based on chosen
      // sign option
      signs[i][c] = sign_options[select][c] == solution[options[c]];
    }
  }
}

// Count how many clauses evaluate to true
int MAXSAT::evaluate(size_t subfunction, const vector<bool> & solution) {
  for (size_t c = 0; c < 3; c++) {
    // if the literal is true, move to the next clause
    if (solution[epistasis_[subfunction][c]] == signs[subfunction][c]) {
      return weights[subfunction];
    }
  }
  return 0;
}

MAXSAT_File::MAXSAT_File(Configuration& config, int run_number) {

  ifstream in(config.get<string>("sat_file"));
  string line;
  while (getline(in, line)) {
    if (line.size() == 0 or line[0] == 'c') {
      // skip blank lines and comments
      continue;
    }
    // found the start of data
    if (line[0] == 'p') {
      break;
    }
  }
  // skip "p cnf "
  istringstream iss(line.substr(6));
  size_t clauses;
  iss >> length_ >> clauses;
  cout << "Length: " << length_ << " Clauses: " << clauses << endl;
  config.set("length", length_);
  int term;
  while (in >> term) {
    vector<size_t> clause(1, abs(term) - 1);
    vector<bool> sign(1, term > 0);
    while (in >> term and term != 0) {
      clause.push_back(abs(term) - 1);
      sign.push_back(term > 0);
    }
    epistasis_.push_back(clause);
    signs.push_back(sign);
  }
  // sanity check
  for (const auto& clause: epistasis_) {
    for (const auto& literal: clause) {
      if (literal >= length_) {
        cout << "Literal too big: " << literal << " " << length_ << endl;
        throw "SHIT";
      }
    }
  }
  if (epistasis_.size() != clauses) {
    cout << "Length mismatch: " << epistasis_.size() << " " << clauses << endl;
    throw "SHIT";
  }
}

// Count how many clauses evaluate to true
int MAXSAT_File::evaluate(size_t subfunction, const vector<bool> & solution) {
  for (size_t c = 0; c < epistasis_[subfunction].size(); c++) {
    // if the literal is true, move to the next clause
    if (solution[epistasis_[subfunction][c]] == signs[subfunction][c]) {
      return 1;
    }
  }
  return 0;
}
