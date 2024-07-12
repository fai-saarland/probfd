#ifndef CEGAR_UTILS_H
#define CEGAR_UTILS_H

#include "downward/task_proxy.h"

#include "downward/utils/hash.h"

#include <memory>
#include <unordered_set>
#include <utility>
#include <vector>

class AbstractTask;

namespace additive_heuristic {
class AdditiveHeuristic;
}

namespace cartesian_abstractions {

/*
  The set of relaxed-reachable facts is the possibly-before set of facts that
  can be reached in the delete-relaxation before 'fact' is reached the first
  time, plus 'fact' itself.
*/
extern utils::HashSet<FactProxy>
get_relaxed_possible_before(const TaskProxy& task, const FactProxy& fact);

extern std::vector<int> get_domain_sizes(const TaskBaseProxy& task);
} // namespace cartesian_abstractions

/*
  TODO: Our proxy classes are meant to be temporary objects and as such
  shouldn't be stored in containers. Once we find a way to avoid
  storing them in containers, we should remove this hashing function.
*/
namespace utils {
inline void feed(HashState& hash_state, const FactProxy& fact)
{
    feed(hash_state, fact.get_pair());
}
} // namespace utils

#endif
