#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_SCCS_TREE_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_SCCS_TREE_H

#include "probfd/merge_and_shrink/merge_strategy_factory.h"

#include "probfd/merge_and_shrink/merge_strategy_factory_sccs_order.h"

namespace probfd::merge_and_shrink {
class MergeTreeFactory;
} // namespace probfd::merge_and_shrink

namespace probfd::merge_and_shrink {

class MergeStrategyFactorySCCsTree : public MergeStrategyFactorySCCs {
    std::shared_ptr<MergeTreeFactory> merge_tree_factory;

public:
    MergeStrategyFactorySCCsTree(
        downward::utils::Verbosity verbosity,
        OrderOfSCCs order_of_sccs,
        std::shared_ptr<MergeTreeFactory> merge_tree_factory);

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
