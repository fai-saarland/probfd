#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_MIASM_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_MIASM_H

#include "probfd/merge_and_shrink/merge_scoring_function.h"

#include "probfd/probabilistic_task.h"

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

    mutable std::vector<std::vector<std::optional<double>>>
        cached_scores_by_merge_candidate_indices;

    mutable downward::utils::LogProxy silent_log;

public:
    MergeScoringFunctionMIASM(
        const ProbabilisticTaskTuple& task,
        bool use_caching,
        std::shared_ptr<ShrinkStrategy> shrink_strategy,
        int max_states,
        int max_states_before_merge,
        int threshold_before_merge);

    std::vector<double> compute_scores(
        const FactoredTransitionSystem& fts,
        const std::vector<std::pair<int, int>>& merge_candidates) override;

    bool requires_liveness() const override { return true; }

    bool requires_goal_distances() const override { return true; }
};

} // namespace probfd::merge_and_shrink

#endif
