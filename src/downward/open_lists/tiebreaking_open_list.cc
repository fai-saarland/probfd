#include "downward/open_lists/tiebreaking_open_list.h"

#include "downward/evaluator.h"
#include "downward/open_list.h"

#include "downward/utils/memory.h"

#include <cassert>
#include <deque>
#include <map>
#include <utility>
#include <vector>

using namespace std;

namespace downward::tiebreaking_open_list {

TieBreakingOpenListFactory::TieBreakingOpenListFactory(
    const vector<shared_ptr<Evaluator>>& evals,
    bool unsafe_pruning,
    bool pref_only)
    : evals(evals)
    , unsafe_pruning(unsafe_pruning)
    , pref_only(pref_only)
{
}

unique_ptr<StateOpenList> TieBreakingOpenListFactory::create_state_open_list(
    const std::shared_ptr<AbstractTask>&)
{
    return std::make_unique<TieBreakingOpenList<StateOpenListEntry>>(
        evals,
        unsafe_pruning,
        pref_only);
}

unique_ptr<EdgeOpenList> TieBreakingOpenListFactory::create_edge_open_list(
    const std::shared_ptr<AbstractTask>&)
{
    return std::make_unique<TieBreakingOpenList<EdgeOpenListEntry>>(
        evals,
        unsafe_pruning,
        pref_only);
}

} // namespace downward::tiebreaking_open_list
