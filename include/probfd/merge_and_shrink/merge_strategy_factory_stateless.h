#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_STATELESS_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_STATELESS_H

#include "probfd/merge_and_shrink/merge_strategy_factory.h"

namespace probfd::merge_and_shrink {
class MergeSelector;
}

namespace probfd::merge_and_shrink {

class MergeStrategyFactoryStateless : public MergeStrategyFactory {
    std::shared_ptr<MergeSelector> merge_selector;

public:
    MergeStrategyFactoryStateless(utils::Verbosity verbosity,
    std::shared_ptr<MergeSelector> merge_selector);

    std::unique_ptr<MergeStrategy> compute_merge_strategy(
        std::shared_ptr<ProbabilisticTask>& task,
        const FactoredTransitionSystem& fts) override;

    bool requires_liveness() const override;
    bool requires_goal_distances() const override;

protected:
    std::string name() const override;

    void dump_strategy_specific_options() const override;
};

} // namespace probfd::merge_and_shrink

#endif // PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_STATELESS_H
