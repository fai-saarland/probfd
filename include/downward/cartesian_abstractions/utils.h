#ifndef CEGAR_UTILS_H
#define CEGAR_UTILS_H

#include "downward/utils/hash.h"

#include <unordered_set>
#include <utility>
#include <vector>

namespace downward {
class VariableSpace;
class ClassicalOperatorSpace;
class State;
struct FactPair;
} // namespace downward

namespace downward::additive_heuristic {
class AdditiveHeuristic;
}

namespace downward::cartesian_abstractions {

/*
  The set of relaxed-reachable facts is the possibly-before set of facts that
  can be reached in the delete-relaxation before 'fact' is reached the first
  time, plus 'fact' itself.
*/
extern utils::HashSet<FactPair> get_relaxed_possible_before(
    const ClassicalOperatorSpace& operators,
    const State& state,
    FactPair fact);

extern std::vector<int> get_domain_sizes(const VariableSpace& variables);
} // namespace downward::cartesian_abstractions

#endif
