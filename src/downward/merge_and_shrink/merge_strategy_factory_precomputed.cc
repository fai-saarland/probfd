#include "downward/merge_and_shrink/merge_strategy_factory_precomputed.h"

#include "downward/merge_and_shrink/merge_strategy_precomputed.h"
#include "downward/merge_and_shrink/merge_tree.h"
#include "downward/merge_and_shrink/merge_tree_factory.h"

#include "downward/utils/memory.h"

using namespace std;

namespace downward::merge_and_shrink {
MergeStrategyFactoryPrecomputed::MergeStrategyFactoryPrecomputed(
    const shared_ptr<MergeTreeFactory>& merge_tree,
    utils::Verbosity verbosity)
    : MergeStrategyFactory(verbosity)
    , merge_tree_factory(merge_tree)
{
}

unique_ptr<MergeStrategy>
MergeStrategyFactoryPrecomputed::compute_merge_strategy(
    const AbstractTask& task,
    const FactoredTransitionSystem& fts)
{
    unique_ptr<MergeTree> merge_tree =
        merge_tree_factory->compute_merge_tree(task);
    return std::make_unique<MergeStrategyPrecomputed>(
        fts,
        std::move(merge_tree));
}

bool MergeStrategyFactoryPrecomputed::requires_init_distances() const
{
    return merge_tree_factory->requires_init_distances();
}

bool MergeStrategyFactoryPrecomputed::requires_goal_distances() const
{
    return merge_tree_factory->requires_goal_distances();
}

string MergeStrategyFactoryPrecomputed::name() const
{
    return "precomputed";
}

void MergeStrategyFactoryPrecomputed::dump_strategy_specific_options() const
{
    if (log.is_at_least_normal()) {
        merge_tree_factory->dump_options(log);
    }
}

} // namespace merge_and_shrink
