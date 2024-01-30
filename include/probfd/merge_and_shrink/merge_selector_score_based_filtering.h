#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_SELECTOR_SCORE_BASED_FILTERING_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_SELECTOR_SCORE_BASED_FILTERING_H

#include "probfd/merge_and_shrink/merge_selector.h"

#include <memory>
#include <vector>

namespace probfd::merge_and_shrink {
class MergeScoringFunction;
}

namespace probfd::merge_and_shrink {

class MergeSelectorScoreBasedFiltering : public MergeSelector {
    std::vector<std::shared_ptr<MergeScoringFunction>> merge_scoring_functions;

public:
    explicit MergeSelectorScoreBasedFiltering(
        std::vector<std::shared_ptr<MergeScoringFunction>> scoring_functions);

    std::pair<int, int> select_merge(
        const FactoredTransitionSystem& fts,
        const std::vector<int>& indices_subset =
            std::vector<int>()) const override;

    void initialize(const ProbabilisticTaskProxy& task_proxy) override;

    bool requires_liveness() const override;
    bool requires_goal_distances() const override;

protected:
    std::string name() const override;

    void dump_selector_specific_options(utils::LogProxy& log) const override;
};

} // namespace probfd::merge_and_shrink

#endif
