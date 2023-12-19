#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_STATELESS_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_STATELESS_H

#include "probfd/merge_and_shrink/merge_strategy_factory.h"

namespace probfd::merge_and_shrink {
class MergeSelector;
}

namespace probfd::merge_and_shrink {

class MergeStrategyFactoryStateless : public MergeStrategyFactory {
    std::shared_ptr<MergeSelector> merge_selector;

protected:
    std::string name() const override;

    void dump_strategy_specific_options() const override;

public:
    explicit MergeStrategyFactoryStateless(const plugins::Options& options);

    std::unique_ptr<MergeStrategy> compute_merge_strategy(
        const ProbabilisticTaskProxy& task_proxy,
        const FactoredTransitionSystem& fts) override;

    bool requires_init_distances() const override;
    bool requires_goal_distances() const override;
};

} // namespace probfd::merge_and_shrink

#endif // PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_STATELESS_H
