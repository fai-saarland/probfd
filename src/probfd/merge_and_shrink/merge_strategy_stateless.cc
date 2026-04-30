#include "probfd/merge_and_shrink/merge_strategy_stateless.h"

#include "probfd/merge_and_shrink/merge_selector.h"

using namespace std;

namespace probfd::merge_and_shrink {

MergeStrategyStateless::MergeStrategyStateless(
    const shared_ptr<MergeSelector>& merge_selector)
    : merge_selector(merge_selector)
{
}

pair<int, int>
MergeStrategyStateless::get_next(const FactoredTransitionSystem& fts)
{ return merge_selector->select_merge(fts); }

bool MergeStrategyStateless::requires_liveness() const
{ return merge_selector->requires_liveness(); }

bool MergeStrategyStateless::requires_goal_distances() const
{ return merge_selector->requires_goal_distances(); }

} // namespace probfd::merge_and_shrink