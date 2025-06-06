#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_MIASM_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_MIASM_H

#include "probfd/merge_and_shrink/merge_scoring_function.h"

#include "downward/utils/logging.h"

#include <memory>
#include <optional>

namespace probfd::merge_and_shrink {
class ShrinkStrategy;
}

namespace probfd::merge_and_shrink {

class MergeScoringFunctionMIASM : public MergeScoringFunction {
    const bool use_caching;
    const std::shared_ptr<ShrinkStrategy> shrink_strategy;
    const int max_states;
    const int max_states_before_merge;
    const int shrink_threshold_before_merge;
    downward::utils::LogProxy silent_log;
    std::vector<std::vector<std::optional<double>>>
        cached_scores_by_merge_candidate_indices;

public:
    MergeScoringFunctionMIASM(
        bool use_caching,
        std::shared_ptr<ShrinkStrategy> shrink_strategy,
        int max_states,
        int max_states_before_merge,
        int threshold_before_merge);

    std::vector<double> compute_scores(
        const FactoredTransitionSystem& fts,
        const std::vector<std::pair<int, int>>& merge_candidates) override;

    void initialize(const ProbabilisticTaskProxy& task_proxy) override;

    bool requires_liveness() const override { return true; }

    bool requires_goal_distances() const override { return true; }

private:
    std::string name() const override;
    void dump_function_specific_options(
        downward::utils::LogProxy& log) const override;
};

} // namespace probfd::merge_and_shrink

#endif
