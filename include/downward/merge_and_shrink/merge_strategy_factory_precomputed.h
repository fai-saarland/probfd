#ifndef MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_PRECOMPUTED_H
#define MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_PRECOMPUTED_H

#include "downward/merge_and_shrink/merge_strategy_factory.h"

namespace downward::merge_and_shrink {
class MergeTreeFactory;

class MergeStrategyFactoryPrecomputed : public MergeStrategyFactory {
    std::shared_ptr<MergeTreeFactory> merge_tree_factory;

protected:
    std::string name() const override;
    void dump_strategy_specific_options() const override;

public:
    MergeStrategyFactoryPrecomputed(
        const std::shared_ptr<MergeTreeFactory>& merge_tree,
        utils::Verbosity verbosity);

    std::unique_ptr<MergeStrategy> compute_merge_strategy(
        const AbstractTaskTuple& task,
        const FactoredTransitionSystem& fts) override;

    bool requires_init_distances() const override;
    bool requires_goal_distances() const override;
};
} // namespace merge_and_shrink

#endif
