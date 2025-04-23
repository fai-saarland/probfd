#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_H

#include <string>
#include <vector>

namespace downward::utils {
class LogProxy;
}

namespace probfd {
class ProbabilisticTask;
}

namespace probfd::merge_and_shrink {
class FactoredTransitionSystem;
}

namespace probfd::merge_and_shrink {

class MergeScoringFunction {
    virtual std::string name() const = 0;

    virtual void
    dump_function_specific_options(downward::utils::LogProxy&) const
    {
    }

protected:
    bool initialized = false;

public:
    virtual ~MergeScoringFunction() = default;

    virtual std::vector<double> compute_scores(
        const FactoredTransitionSystem& fts,
        const std::vector<std::pair<int, int>>& merge_candidates) = 0;

    virtual bool requires_liveness() const = 0;
    virtual bool requires_goal_distances() const = 0;

    // Overriding methods must set initialized to true.
    virtual void initialize(const ProbabilisticTask&)
    {
        initialized = true;
    }

    void dump_options(downward::utils::LogProxy& log) const;
};

} // namespace probfd::merge_and_shrink

#endif // PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_H
