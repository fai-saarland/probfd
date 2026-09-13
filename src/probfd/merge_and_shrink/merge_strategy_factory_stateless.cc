#include "probfd/merge_and_shrink/merge_strategy_factory_stateless.h"

#include "probfd/merge_and_shrink/merge_selector.h"
#include "probfd/merge_and_shrink/merge_selector_factory.h"
#include "probfd/merge_and_shrink/merge_strategy_stateless.h"

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

MergeStrategyFactoryStateless::MergeStrategyFactoryStateless(
    utils::Verbosity verbosity,
    std::shared_ptr<MergeSelectorFactory> merge_selector_factory)
    : MergeStrategyFactory(verbosity)
    , merge_selector_factory(std::move(merge_selector_factory))
{
}

unique_ptr<MergeStrategy> MergeStrategyFactoryStateless::compute_merge_strategy(
    const SharedProbabilisticTask&,
    const FactoredTransitionSystem& fts)
{
    auto merge_selector = merge_selector_factory->compute_selector(fts);

    return std::make_unique<MergeStrategyStateless>(std::move(merge_selector));
}

string MergeStrategyFactoryStateless::name() const
{
    return "stateless";
}

void MergeStrategyFactoryStateless::dump_strategy_specific_options() const
{
    if (log.is_at_least_normal()) {
        merge_selector_factory->dump_options(log);
    }
}

bool MergeStrategyFactoryStateless::requires_liveness() const
{
    return merge_selector_factory->requires_liveness();
}

bool MergeStrategyFactoryStateless::requires_goal_distances() const
{
    return merge_selector_factory->requires_goal_distances();
}

} // namespace probfd::merge_and_shrink