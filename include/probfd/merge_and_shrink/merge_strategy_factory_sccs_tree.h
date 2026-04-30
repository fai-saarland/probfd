#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_SCCS_TREE_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_SCCS_TREE_H

#include "probfd/merge_and_shrink/merge_strategy_factory.h"

namespace probfd::merge_and_shrink {
class MergeTreeFactory;
} // namespace probfd::merge_and_shrink

namespace probfd::merge_and_shrink {

enum class OrderOfSCCs {
    TOPOLOGICAL,
    REVERSE_TOPOLOGICAL,
    DECREASING,
    INCREASING
};

class MergeStrategyFactorySCCsTree : public MergeStrategyFactory {
    OrderOfSCCs order_of_sccs;
    std::shared_ptr<MergeTreeFactory> merge_tree_factory;

public:
    MergeStrategyFactorySCCsTree(
        OrderOfSCCs order_of_sccs,
        std::shared_ptr<MergeTreeFactory> merge_tree_factory);

    std::unique_ptr<MergeStrategy> compute_merge_strategy(
        const SharedProbabilisticTask& task,
        const FactoredTransitionSystem& fts,
        downward::utils::LogProxy& log) override;

protected:
    std::string name() const override;

    void dump_strategy_specific_options(
        downward::utils::LogProxy& log) const override;
};

} // namespace probfd::merge_and_shrink

#endif
