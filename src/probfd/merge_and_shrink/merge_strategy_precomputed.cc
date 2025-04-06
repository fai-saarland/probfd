#include "probfd/merge_and_shrink/merge_strategy_precomputed.h"

#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/merge_tree.h"

#include <cassert>

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

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
    const int next_merge_index = fts.get_size();
    auto [left, right] = merge_tree->get_next_merge(next_merge_index);
    assert(fts.is_active(left) && fts.is_active(right));
    return {left, right};
}

} // namespace probfd::merge_and_shrink