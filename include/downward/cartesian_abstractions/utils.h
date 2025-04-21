#ifndef CEGAR_UTILS_H
#define CEGAR_UTILS_H

#include "downward/task_proxy.h"

#include "downward/utils/hash.h"

#include <memory>
#include <unordered_set>
#include <utility>
#include <vector>

namespace downward {
class AbstractTask;
struct FactPair;
}

namespace downward::additive_heuristic {
class AdditiveHeuristic;
}

namespace downward::cartesian_abstractions {

/*
  The set of relaxed-reachable facts is the possibly-before set of facts that
  can be reached in the delete-relaxation before 'fact' is reached the first
  time, plus 'fact' itself.
*/
extern utils::HashSet<FactPair>
get_relaxed_possible_before(const TaskProxy& task, FactPair fact);

extern std::vector<int> get_domain_sizes(const VariablesProxy& variables);
} // namespace cartesian_abstractions

#endif
