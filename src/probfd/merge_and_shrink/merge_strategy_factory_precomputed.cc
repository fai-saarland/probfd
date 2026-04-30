#include "probfd/merge_and_shrink/merge_strategy_factory_precomputed.h"

#include "probfd/merge_and_shrink/merge_strategy_precomputed.h"
#include "probfd/merge_and_shrink/merge_tree.h"
#include "probfd/merge_and_shrink/merge_tree_factory.h"

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

MergeStrategyFactoryPrecomputed::MergeStrategyFactoryPrecomputed(
    std::shared_ptr<MergeTreeFactory> merge_tree_factory)
    : merge_tree_factory(std::move(merge_tree_factory))
{
}

unique_ptr<MergeStrategy>
MergeStrategyFactoryPrecomputed::compute_merge_strategy(
    const SharedProbabilisticTask& task,
    const FactoredTransitionSystem&,
    utils::LogProxy&)
{
    return std::make_unique<MergeStrategyPrecomputed>(
        merge_tree_factory->compute_merge_tree(task));
}

string MergeStrategyFactoryPrecomputed::name() const
{ return "precomputed"; }

void MergeStrategyFactoryPrecomputed::dump_strategy_specific_options(
    utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) { merge_tree_factory->dump_options(log); }
}

} // namespace probfd::merge_and_shrink