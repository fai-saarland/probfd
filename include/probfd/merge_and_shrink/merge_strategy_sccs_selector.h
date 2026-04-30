#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_SCCS_SELECTOR_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_STRATEGY_SCCS_SELECTOR_H

#include "probfd/merge_and_shrink/merge_strategy.h"

#include "probfd/probabilistic_task.h"

#include <memory>
#include <vector>

namespace probfd::merge_and_shrink {
class MergeSelector;
} // namespace probfd::merge_and_shrink

namespace probfd::merge_and_shrink {

class MergeStrategySCCsSelector : public MergeStrategy {
    std::shared_ptr<MergeSelector> merge_selector;
    std::vector<std::vector<int>> non_singleton_cg_sccs;

    std::vector<int> current_ts_indices;

public:
    MergeStrategySCCsSelector(
        const std::shared_ptr<MergeSelector>& merge_selector,
        std::vector<std::vector<int>>&& non_singleton_cg_sccs);

    ~MergeStrategySCCsSelector() override;

    std::pair<int, int> get_next(const FactoredTransitionSystem& fts) override;

    bool requires_liveness() const override;
    bool requires_goal_distances() const override;
};

} // namespace probfd::merge_and_shrink

#endif
