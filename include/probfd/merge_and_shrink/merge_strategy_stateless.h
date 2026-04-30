#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_STATELESS_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_STATELESS_H

#include "probfd/merge_and_shrink/merge_strategy.h"

#include <memory>

namespace probfd::merge_and_shrink {
class MergeSelector;
}

namespace probfd::merge_and_shrink {

class MergeStrategyStateless : public MergeStrategy {
    const std::shared_ptr<MergeSelector> merge_selector;

public:
    explicit MergeStrategyStateless(
        const std::shared_ptr<MergeSelector>& merge_selector);

    std::pair<int, int> get_next(const FactoredTransitionSystem& fts) override;

    bool requires_liveness() const override;
    bool requires_goal_distances() const override;
};

} // namespace probfd::merge_and_shrink

#endif
