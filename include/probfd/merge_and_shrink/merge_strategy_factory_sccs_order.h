#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_SCCS_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_SCCS_H

#include "probfd/merge_and_shrink/merge_strategy_factory.h"

namespace probfd::merge_and_shrink {
class MergeSelectorFactory;
} // namespace probfd::merge_and_shrink

namespace probfd::merge_and_shrink {

enum class OrderOfSCCs {
    TOPOLOGICAL,
    REVERSE_TOPOLOGICAL,
    DECREASING,
    INCREASING
};

class MergeStrategyFactorySCCs : public MergeStrategyFactory {
protected:
    OrderOfSCCs order_of_sccs;

    explicit MergeStrategyFactorySCCs(
        downward::utils::Verbosity verbosity,
        OrderOfSCCs order_of_sccs);
};

} // namespace probfd::merge_and_shrink

#endif
