#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_H

#include <utility>
#include <vector>

namespace probfd::merge_and_shrink {
class FactoredTransitionSystem;
}

namespace probfd::merge_and_shrink {

class MergeScoringFunction {
public:
    virtual ~MergeScoringFunction() = default;

    virtual std::vector<double> compute_scores(
        const FactoredTransitionSystem& fts,
        const std::vector<std::pair<int, int>>& merge_candidates) = 0;

    virtual bool requires_liveness() const = 0;
    virtual bool requires_goal_distances() const = 0;
};

} // namespace probfd::merge_and_shrink

#endif // PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_H
