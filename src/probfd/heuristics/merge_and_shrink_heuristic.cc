#include "probfd/heuristics/merge_and_shrink_heuristic.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/factored_mapping.h"
#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/task_heuristic_factory_category.h"

using namespace std;
using namespace downward;
using namespace probfd::merge_and_shrink;

namespace probfd::heuristics {

FactorDistances::FactorDistances(
    std::unique_ptr<FactoredMapping> factored_mapping,
    Distances& distances)
    : factored_mapping(std::move(factored_mapping))
    , distance_table(distances.extract_goal_distances())
{
}

FactorDistances::FactorDistances(
    std::unique_ptr<FactoredMapping> factored_mapping,
    const Labels& labels,
    const TransitionSystem& ts)
    : factored_mapping(std::move(factored_mapping))
    , distance_table(ts.get_size(), DISTANCE_UNKNOWN)
{
    compute_goal_distances(labels, ts, distance_table);
}

FactorDistances::~FactorDistances() = default;
FactorDistances::FactorDistances(FactorDistances&&) noexcept = default;
FactorDistances&
FactorDistances::operator=(FactorDistances&&) noexcept = default;

value_t FactorDistances::lookup_distance(const State& state) const
{
    const int abs = factored_mapping->get_abstract_state(state);
    return abs == PRUNED_STATE ? INFINITE_VALUE : distance_table[abs];
}

MergeAndShrinkHeuristic::MergeAndShrinkHeuristic(
    std::vector<FactorDistances> factor_distances)
    : factor_distances_(std::move(factor_distances))
{
}

MergeAndShrinkHeuristic::~MergeAndShrinkHeuristic() = default;

value_t MergeAndShrinkHeuristic::evaluate(const State& state) const
{
    value_t heuristic = 0;

    for (const auto& distances : factor_distances_) {
        const value_t cost = distances.lookup_distance(state);

        if (cost == INFINITE_VALUE) {
            // If state is unreachable or irrelevant, we encountered a dead end.
            return INFINITE_VALUE;
        }

        heuristic = max(heuristic, cost);
    }

    return heuristic;
}

} // namespace probfd::heuristics