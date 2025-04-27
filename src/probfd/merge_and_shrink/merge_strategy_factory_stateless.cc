#include "probfd/merge_and_shrink/merge_strategy_factory_stateless.h"

#include "probfd/merge_and_shrink/merge_selector.h"
#include "probfd/merge_and_shrink/merge_strategy_stateless.h"

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

MergeStrategyFactoryStateless::MergeStrategyFactoryStateless(
    utils::Verbosity verbosity,
    std::shared_ptr<MergeSelector> merge_selector)
    : MergeStrategyFactory(verbosity)
    , merge_selector(std::move(merge_selector))
{
}

unique_ptr<MergeStrategy> MergeStrategyFactoryStateless::compute_merge_strategy(
    const SharedProbabilisticTask& task,
    const FactoredTransitionSystem& fts)
{
    merge_selector->initialize(to_refs(task));
    return std::make_unique<MergeStrategyStateless>(fts, merge_selector);
}

string MergeStrategyFactoryStateless::name() const
{
    return "stateless";
}

void MergeStrategyFactoryStateless::dump_strategy_specific_options() const
{
    if (log.is_at_least_normal()) {
        merge_selector->dump_options(log);
    }
}

bool MergeStrategyFactoryStateless::requires_liveness() const
{
    return merge_selector->requires_liveness();
}

bool MergeStrategyFactoryStateless::requires_goal_distances() const
{
    return merge_selector->requires_goal_distances();
}

} // namespace probfd::merge_and_shrink