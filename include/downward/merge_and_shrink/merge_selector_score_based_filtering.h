#ifndef MERGE_AND_SHRINK_MERGE_SELECTOR_SCORE_BASED_FILTERING_H
#define MERGE_AND_SHRINK_MERGE_SELECTOR_SCORE_BASED_FILTERING_H

#include "downward/merge_and_shrink/merge_selector.h"

#include <memory>
#include <vector>

namespace downward::merge_and_shrink {
class MergeScoringFunction;

class MergeSelectorScoreBasedFiltering : public MergeSelector {
    std::vector<std::shared_ptr<MergeScoringFunction>> merge_scoring_functions;

protected:
    std::string name() const override;
    void dump_selector_specific_options(utils::LogProxy& log) const override;

public:
    explicit MergeSelectorScoreBasedFiltering(
        const std::vector<std::shared_ptr<MergeScoringFunction>>&
            scoring_functions);
    std::pair<int, int> select_merge(
        const FactoredTransitionSystem& fts,
        const std::vector<int>& indices_subset =
            std::vector<int>()) const override;
    void initialize(const AbstractTaskTuple& task) override;
    bool requires_init_distances() const override;
    bool requires_goal_distances() const override;
};
} // namespace downward::merge_and_shrink

#endif
