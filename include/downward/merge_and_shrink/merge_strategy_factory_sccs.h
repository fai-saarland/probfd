#ifndef MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_SCCS_H
#define MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_SCCS_H

#include "downward/merge_and_shrink/merge_strategy_factory.h"

namespace downward::merge_and_shrink {
class MergeSelector;

enum class OrderOfSCCs {
    TOPOLOGICAL,
    REVERSE_TOPOLOGICAL,
    DECREASING,
    INCREASING
};

class MergeStrategyFactorySCCs : public MergeStrategyFactory {
    OrderOfSCCs order_of_sccs;
    std::shared_ptr<MergeSelector> merge_selector;

protected:
    virtual std::string name() const override;
    virtual void dump_strategy_specific_options() const override;

public:
    MergeStrategyFactorySCCs(
        const OrderOfSCCs& order_of_sccs,
        const std::shared_ptr<MergeSelector>& merge_selector,
        utils::Verbosity verbosity);
    virtual std::unique_ptr<MergeStrategy> compute_merge_strategy(
        const TaskProxy& task_proxy,
        const FactoredTransitionSystem& fts) override;
    virtual bool requires_init_distances() const override;
    virtual bool requires_goal_distances() const override;
};
} // namespace merge_and_shrink

#endif
