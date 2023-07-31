#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_AND_SHRINK_HEURISTIC_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_AND_SHRINK_HEURISTIC_H

#include "probfd/heuristics/task_dependent_heuristic.h"

namespace probfd::merge_and_shrink {
class FactoredTransitionSystem;
class MergeAndShrinkDistanceRepresentation;
class MergeAndShrinkAlgorithm;
} // namespace probfd::merge_and_shrink

namespace probfd::heuristics {

class MergeAndShrinkHeuristic : public TaskDependentHeuristic {
    // The final merge-and-shrink representations, storing goal distances.
    std::vector<
        std::unique_ptr<merge_and_shrink::MergeAndShrinkDistanceRepresentation>>
        mas_representations;

    void
    extract_factor(merge_and_shrink::FactoredTransitionSystem& fts, int index);
    bool
    extract_unsolvable_factor(merge_and_shrink::FactoredTransitionSystem& fts);
    void
    extract_nontrivial_factors(merge_and_shrink::FactoredTransitionSystem& fts);
    void extract_factors(merge_and_shrink::FactoredTransitionSystem& fts);

public:
    MergeAndShrinkHeuristic(
        merge_and_shrink::MergeAndShrinkAlgorithm& algorithm,
        std::shared_ptr<ProbabilisticTask> task,
        utils::LogProxy log);

    value_t evaluate(const State& state) const override;
};

} // namespace probfd::heuristics

#endif
