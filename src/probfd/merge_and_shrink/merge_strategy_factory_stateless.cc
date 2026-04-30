#include "probfd/merge_and_shrink/merge_strategy_factory_stateless.h"

#include "probfd/merge_and_shrink/merge_selector.h"
#include "probfd/merge_and_shrink/merge_selector_factory.h"
#include "probfd/merge_and_shrink/merge_strategy_stateless.h"

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

MergeStrategyFactoryStateless::MergeStrategyFactoryStateless(
    std::shared_ptr<MergeSelectorFactory> merge_selector)
    : merge_selector(std::move(merge_selector))
{
}

unique_ptr<MergeStrategy> MergeStrategyFactoryStateless::compute_merge_strategy(
    const SharedProbabilisticTask& task,
    const FactoredTransitionSystem&,
    utils::LogProxy&)
{
    auto ms = merge_selector->create(task);
    return std::make_unique<MergeStrategyStateless>(std::move(ms));
}

string MergeStrategyFactoryStateless::name() const
{ return "stateless"; }

void MergeStrategyFactoryStateless::dump_strategy_specific_options(
    utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) { merge_selector->dump_options(log); }
}

} // namespace probfd::merge_and_shrink