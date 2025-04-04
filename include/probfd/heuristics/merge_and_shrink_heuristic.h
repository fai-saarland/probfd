#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_AND_SHRINK_HEURISTIC_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_AND_SHRINK_HEURISTIC_H

#include "probfd/aliases.h"
#include "probfd/fdr_types.h"
#include "probfd/heuristic.h"

namespace probfd::merge_and_shrink {
class Distances;
class Labels;
class TransitionSystem;
class FactoredMapping;
class FactoredTransitionSystem;
} // namespace probfd::merge_and_shrink

namespace probfd::heuristics {

struct FactorDistances {
    std::unique_ptr<merge_and_shrink::FactoredMapping> factored_mapping;
    std::vector<value_t> distance_table;

    FactorDistances(
        std::unique_ptr<merge_and_shrink::FactoredMapping> factored_mapping,
        merge_and_shrink::Distances& distances);

    FactorDistances(
        std::unique_ptr<merge_and_shrink::FactoredMapping> factored_mapping,
        const merge_and_shrink::Labels& labels,
        const merge_and_shrink::TransitionSystem& ts);

    value_t lookup_distance(const State& state) const;
};

class MergeAndShrinkHeuristic final : public FDREvaluator {
    // The final merge-and-shrink representations, storing goal distances.
    std::vector<FactorDistances> factor_distances_;

public:
    explicit MergeAndShrinkHeuristic(
        std::vector<FactorDistances> factor_distances);

    ~MergeAndShrinkHeuristic() override;

    value_t evaluate(const State& state) const override;
};

} // namespace probfd::heuristics

#endif
