#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_SELECTOR_SCORE_BASED_FILTERING_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_SELECTOR_SCORE_BASED_FILTERING_H

#include "probfd/merge_and_shrink/merge_selector.h"

#include <memory>
#include <vector>

namespace plugins {
class Options;
}

namespace probfd::merge_and_shrink {
class MergeScoringFunction;
}

namespace probfd::merge_and_shrink {

class MergeSelectorScoreBasedFiltering : public MergeSelector {
    std::vector<std::shared_ptr<MergeScoringFunction>> merge_scoring_functions;

protected:
    std::string name() const override;

    void dump_selector_specific_options(utils::LogProxy& log) const override;

public:
    explicit MergeSelectorScoreBasedFiltering(const plugins::Options& options);

    std::pair<int, int> select_merge(
        const FactoredTransitionSystem& fts,
        const std::vector<int>& indices_subset =
            std::vector<int>()) const override;

    void initialize(const ProbabilisticTaskProxy& task_proxy) override;

    bool requires_init_distances() const override;
    bool requires_goal_distances() const override;
};

} // namespace probfd::merge_and_shrink

#endif // PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_SELECTOR_SCORE_BASED_FILTERING_H
