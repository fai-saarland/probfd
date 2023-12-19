#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_STRATEGY_PRECOMPUTED_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_STRATEGY_PRECOMPUTED_H

#include "probfd/merge_and_shrink/merge_strategy.h"

#include <memory>

namespace probfd::merge_and_shrink {
class MergeTree;
}

namespace probfd::merge_and_shrink {
class MergeStrategyPrecomputed : public MergeStrategy {
    std::unique_ptr<MergeTree> merge_tree;

public:
    MergeStrategyPrecomputed(
        const FactoredTransitionSystem& fts,
        std::unique_ptr<MergeTree> merge_tree);
    ~MergeStrategyPrecomputed() override;

    std::pair<int, int> get_next() override;
};

} // namespace probfd::merge_and_shrink

#endif // PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_STRATEGY_PRECOMPUTED_H
