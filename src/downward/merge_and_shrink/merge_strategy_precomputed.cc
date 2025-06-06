#include "downward/merge_and_shrink/merge_strategy_precomputed.h"

#include "downward/merge_and_shrink/factored_transition_system.h"
#include "downward/merge_and_shrink/merge_tree.h"

#include <cassert>

using namespace std;

namespace downward::merge_and_shrink {
MergeStrategyPrecomputed::MergeStrategyPrecomputed(
    const FactoredTransitionSystem& fts,
    unique_ptr<MergeTree> merge_tree)
    : MergeStrategy(fts)
    , merge_tree(std::move(merge_tree))
{
}

MergeStrategyPrecomputed::~MergeStrategyPrecomputed() = default;

pair<int, int> MergeStrategyPrecomputed::get_next()
{
    assert(!merge_tree->done());
    int next_merge_index = fts.get_size();
    pair<int, int> next_merge = merge_tree->get_next_merge(next_merge_index);
    assert(fts.is_active(next_merge.first));
    assert(fts.is_active(next_merge.second));
    return next_merge;
}
} // namespace merge_and_shrink
