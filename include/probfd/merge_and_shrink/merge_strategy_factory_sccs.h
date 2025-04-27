#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_SCCS_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_SCCS_H

#include "probfd/merge_and_shrink/merge_strategy_factory.h"

namespace probfd::merge_and_shrink {
class MergeTreeFactory;
class MergeSelector;
} // namespace probfd::merge_and_shrink

namespace probfd::merge_and_shrink {

enum class OrderOfSCCs {
    TOPOLOGICAL,
    REVERSE_TOPOLOGICAL,
    DECREASING,
    INCREASING
};

class MergeStrategyFactorySCCs : public MergeStrategyFactory {
    OrderOfSCCs order_of_sccs;
    std::shared_ptr<MergeTreeFactory> merge_tree_factory;
    std::shared_ptr<MergeSelector> merge_selector;

public:
    MergeStrategyFactorySCCs(
        downward::utils::Verbosity verbosity,
        OrderOfSCCs order_of_sccs,
        std::shared_ptr<MergeTreeFactory> merge_tree_factory,
        std::shared_ptr<MergeSelector> merge_selector);

    std::unique_ptr<MergeStrategy> compute_merge_strategy(
        const SharedProbabilisticTask& task,
        const FactoredTransitionSystem& fts) override;

    bool requires_liveness() const override;
    bool requires_goal_distances() const override;

protected:
    std::string name() const override;
    void dump_strategy_specific_options() const override;
};

} // namespace probfd::merge_and_shrink

#endif
