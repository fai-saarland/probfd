#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_SINGLE_RANDOM_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_SINGLE_RANDOM_H

#include "probfd/merge_and_shrink/merge_scoring_function.h"

#include <memory>

namespace plugins {
class Options;
}

namespace utils {
class RandomNumberGenerator;
}

namespace probfd::merge_and_shrink {

class MergeScoringFunctionSingleRandom : public MergeScoringFunction {
    int random_seed; // only for dump options
    std::shared_ptr<utils::RandomNumberGenerator> rng;

public:
    explicit MergeScoringFunctionSingleRandom(int random_seed);

    std::vector<double> compute_scores(
        const FactoredTransitionSystem& fts,
        const std::vector<std::pair<int, int>>& merge_candidates) override;

    bool requires_init_distances() const override { return false; }
    bool requires_goal_distances() const override { return false; }

private:
    std::string name() const override;
    void dump_function_specific_options(utils::LogProxy& log) const override;
};

} // namespace probfd::merge_and_shrink

#endif // PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_SINGLE_RANDOM_H
