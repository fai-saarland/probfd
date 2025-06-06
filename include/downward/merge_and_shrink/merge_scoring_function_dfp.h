#ifndef MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_DFP_H
#define MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_DFP_H

#include "downward/merge_and_shrink/merge_scoring_function.h"

namespace downward::merge_and_shrink {
class MergeScoringFunctionDFP : public MergeScoringFunction {
    virtual std::string name() const override;

public:
    MergeScoringFunctionDFP() = default;
    virtual std::vector<double> compute_scores(
        const FactoredTransitionSystem& fts,
        const std::vector<std::pair<int, int>>& merge_candidates) override;

    virtual bool requires_init_distances() const override { return false; }

    virtual bool requires_goal_distances() const override { return true; }
};
} // namespace merge_and_shrink

#endif
