#ifndef MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_SINGLE_RANDOM_H
#define MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_SINGLE_RANDOM_H

#include "downward/merge_and_shrink/merge_scoring_function.h"

#include <memory>

namespace downward::utils {
class RandomNumberGenerator;
}

namespace downward::merge_and_shrink {
class MergeScoringFunctionSingleRandom : public MergeScoringFunction {
    std::shared_ptr<utils::RandomNumberGenerator> rng;

    virtual std::string name() const override;
    virtual void
    dump_function_specific_options(utils::LogProxy& log) const override;

public:
    explicit MergeScoringFunctionSingleRandom(
        std::shared_ptr<utils::RandomNumberGenerator> rng);

    virtual std::vector<double> compute_scores(
        const FactoredTransitionSystem& fts,
        const std::vector<std::pair<int, int>>& merge_candidates) override;

    virtual bool requires_init_distances() const override { return false; }

    virtual bool requires_goal_distances() const override { return false; }
};
} // namespace downward::merge_and_shrink

#endif
