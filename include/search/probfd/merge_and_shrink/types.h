#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_TYPES_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_TYPES_H

#include <forward_list>
#include <list>
#include <vector>

namespace probfd::merge_and_shrink {

static constexpr int PRUNED_STATE = -1;

/*
  An equivalence class is a set of abstract states that shall be
  mapped (shrunk) to the same abstract state.

  An equivalence relation is a partitioning of states into
  equivalence classes. It may omit certain states entirely; these
  will be dropped completely and receive an h value of infinity.
*/
using StateEquivalenceClass = std::forward_list<int>;
using StateEquivalenceRelation = std::vector<StateEquivalenceClass>;

} // namespace probfd::merge_and_shrink

#endif
