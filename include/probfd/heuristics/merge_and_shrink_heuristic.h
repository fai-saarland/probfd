#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_AND_SHRINK_HEURISTIC_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_AND_SHRINK_HEURISTIC_H

#include "probfd/heuristics/task_dependent_heuristic.h"

namespace probfd::merge_and_shrink {
class FactoredTransitionSystem;
class MergeAndShrinkAlgorithm;
} // namespace probfd::merge_and_shrink

namespace probfd::heuristics {

class MergeAndShrinkHeuristic : public TaskDependentHeuristic {
    struct FactorDistances;

    // The final merge-and-shrink representations, storing goal distances.
    std::vector<FactorDistances> factor_distances;

    void
    extract_factor(merge_and_shrink::FactoredTransitionSystem& fts, int index);
    bool
    extract_unsolvable_factor(merge_and_shrink::FactoredTransitionSystem& fts);

public:
    MergeAndShrinkHeuristic(
        merge_and_shrink::MergeAndShrinkAlgorithm& algorithm,
        std::shared_ptr<ProbabilisticTask> task,
        utils::LogProxy log);

    ~MergeAndShrinkHeuristic();

    value_t evaluate(const State& state) const override;
};

} // namespace probfd::heuristics

#endif
