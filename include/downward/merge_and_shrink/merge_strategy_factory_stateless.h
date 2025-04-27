#ifndef MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_STATELESS_H
#define MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_STATELESS_H

#include "downward/merge_and_shrink/merge_strategy_factory.h"

namespace downward::merge_and_shrink {
class MergeSelector;

class MergeStrategyFactoryStateless : public MergeStrategyFactory {
    std::shared_ptr<MergeSelector> merge_selector;

protected:
    std::string name() const override;
    void dump_strategy_specific_options() const override;

public:
    MergeStrategyFactoryStateless(
        const std::shared_ptr<MergeSelector>& merge_selector,
        utils::Verbosity verbosity);

    std::unique_ptr<MergeStrategy> compute_merge_strategy(
        const AbstractTaskTuple& task,
        const FactoredTransitionSystem& fts) override;

    bool requires_init_distances() const override;
    bool requires_goal_distances() const override;
};
} // namespace merge_and_shrink

#endif
