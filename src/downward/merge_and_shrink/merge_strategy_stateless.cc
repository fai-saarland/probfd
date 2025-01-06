#include "downward/merge_and_shrink/merge_strategy_stateless.h"

#include "downward/merge_and_shrink/merge_selector.h"

using namespace std;

namespace merge_and_shrink {
MergeStrategyStateless::MergeStrategyStateless(
    const FactoredTransitionSystem& fts,
    const shared_ptr<MergeSelector>& merge_selector)
    : MergeStrategy(fts)
    , merge_selector(merge_selector)
{
}

pair<int, int> MergeStrategyStateless::get_next()
{
    return merge_selector->select_merge(fts);
}
} // namespace merge_and_shrink
