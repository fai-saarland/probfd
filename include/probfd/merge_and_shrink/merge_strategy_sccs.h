#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_SCCS_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_SCCS_H

#include "probfd/merge_and_shrink/merge_strategy.h"
#include "probfd/probabilistic_task.h"

#include <memory>
#include <vector>

namespace probfd::merge_and_shrink {
class MergeSelector;
class MergeTreeFactory;
class MergeTree;
} // namespace probfd::merge_and_shrink

namespace probfd::merge_and_shrink {

class MergeStrategySCCsTree : public MergeStrategy {
    SharedProbabilisticTask task;

    std::shared_ptr<MergeTreeFactory> merge_tree_factory;
    std::vector<std::vector<int>> non_singleton_cg_sccs;

    std::unique_ptr<MergeTree> current_merge_tree;
    std::vector<int> current_ts_indices;

public:
    MergeStrategySCCsTree(
        const FactoredTransitionSystem& fts,
        SharedProbabilisticTask task,
        const std::shared_ptr<MergeTreeFactory>& merge_tree_factory,
        std::vector<std::vector<int>>&& non_singleton_cg_sccs);

    ~MergeStrategySCCsTree() override;

    std::pair<int, int> get_next() override;
};

class MergeStrategySCCsSelector : public MergeStrategy {
    SharedProbabilisticTask task;

    std::shared_ptr<MergeTreeFactory> merge_tree_factory;
    std::shared_ptr<MergeSelector> merge_selector;
    std::vector<std::vector<int>> non_singleton_cg_sccs;

    std::vector<int> current_ts_indices;

public:
    MergeStrategySCCsSelector(
        const FactoredTransitionSystem& fts,
        SharedProbabilisticTask task,
        const std::shared_ptr<MergeSelector>& merge_selector,
        std::vector<std::vector<int>>&& non_singleton_cg_sccs);

    ~MergeStrategySCCsSelector() override;

    std::pair<int, int> get_next() override;
};

} // namespace probfd::merge_and_shrink

#endif
