#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_PRECOMPUTED_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_PRECOMPUTED_H

#include "probfd/merge_and_shrink/merge_strategy.h"

#include <memory>

namespace probfd::merge_and_shrink {
class MergeTree;
}

namespace probfd::merge_and_shrink {
class MergeStrategyPrecomputed : public MergeStrategy {
    std::unique_ptr<MergeTree> merge_tree;

public:
    explicit MergeStrategyPrecomputed(std::unique_ptr<MergeTree> merge_tree);

    ~MergeStrategyPrecomputed() override;

    std::pair<int, int> get_next(const FactoredTransitionSystem& fts) override;

    bool requires_liveness() const override;
    bool requires_goal_distances() const override;
};

} // namespace probfd::merge_and_shrink

#endif
