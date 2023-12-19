#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_STRATEGY_STATELESS_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_STRATEGY_STATELESS_H

#include "probfd/merge_and_shrink/merge_strategy.h"

#include <memory>

namespace probfd::merge_and_shrink {
class MergeSelector;
}

namespace probfd::merge_and_shrink {

class MergeStrategyStateless : public MergeStrategy {
    const std::shared_ptr<MergeSelector> merge_selector;

public:
    MergeStrategyStateless(
        const FactoredTransitionSystem& fts,
        const std::shared_ptr<MergeSelector>& merge_selector);

    std::pair<int, int> get_next() override;
};

} // namespace probfd::merge_and_shrink

#endif // PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_STRATEGY_STATELESS_H
