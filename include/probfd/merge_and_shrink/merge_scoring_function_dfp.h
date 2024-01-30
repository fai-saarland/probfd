#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_DFP_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_DFP_H

#include "probfd/merge_and_shrink/merge_scoring_function.h"

namespace probfd::merge_and_shrink {

class MergeScoringFunctionDFP : public MergeScoringFunction {
    std::string name() const override;

public:
    std::vector<double> compute_scores(
        const FactoredTransitionSystem& fts,
        const std::vector<std::pair<int, int>>& merge_candidates) override;

    bool requires_liveness() const override { return false; }
    bool requires_goal_distances() const override { return true; }
};

} // namespace probfd::merge_and_shrink

#endif // PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_DFP_H
