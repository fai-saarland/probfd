#include "downward/merge_and_shrink/merge_strategy_factory_stateless.h"

#include "downward/merge_and_shrink/merge_selector.h"
#include "downward/merge_and_shrink/merge_strategy_stateless.h"

#include "downward/utils/memory.h"

using namespace std;

namespace downward::merge_and_shrink {
MergeStrategyFactoryStateless::MergeStrategyFactoryStateless(
    const shared_ptr<MergeSelector>& merge_selector,
    utils::Verbosity verbosity)
    : MergeStrategyFactory(verbosity)
    , merge_selector(merge_selector)
{
}

unique_ptr<MergeStrategy> MergeStrategyFactoryStateless::compute_merge_strategy(
    const AbstractTask& task,
    const FactoredTransitionSystem& fts)
{
    merge_selector->initialize(task);
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

bool MergeStrategyFactoryStateless::requires_init_distances() const
{
    return merge_selector->requires_init_distances();
}

bool MergeStrategyFactoryStateless::requires_goal_distances() const
{
    return merge_selector->requires_goal_distances();
}

} // namespace merge_and_shrink
