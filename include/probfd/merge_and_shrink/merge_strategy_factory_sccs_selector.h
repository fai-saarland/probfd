#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_SCCS_SELECTOR_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_SCCS_SELECTOR_H

#include "probfd/merge_and_shrink/merge_strategy_factory.h"

namespace probfd::merge_and_shrink {
class MergeSelectorFactory;
} // namespace probfd::merge_and_shrink

namespace probfd::merge_and_shrink {

class MergeStrategyFactorySCCsSelector : public MergeStrategyFactory {
public:
    enum class OrderOfSCCs {
        TOPOLOGICAL,
        REVERSE_TOPOLOGICAL,
        DECREASING,
        INCREASING
    };

private:
    OrderOfSCCs order_of_sccs;
    std::shared_ptr<MergeSelectorFactory> merge_selector;

public:
    MergeStrategyFactorySCCsSelector(
        OrderOfSCCs order_of_sccs,
        std::shared_ptr<MergeSelectorFactory> merge_selector);

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
