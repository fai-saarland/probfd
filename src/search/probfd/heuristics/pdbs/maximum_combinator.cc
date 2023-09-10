#include "probfd/heuristics/pdbs/maximum_combinator.h"

#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

void MaximumCombinator::update(const PPDBCollection&)
{
}

int MaximumCombinator::count_improvements_if_added(
    const PPDBCollection& pdbs,
    const ProbabilityAwarePatternDatabase& new_pdb,
    const std::vector<State>& states,
    value_t termination_cost)
{
    int count = 0;

    for (const auto& sample : states) {
        const value_t h_pattern = new_pdb.lookup_estimate(sample);

        if (h_pattern == termination_cost) {
            ++count;
            continue;
        }

        // h_collection: h-value of the current collection heuristic
        for (const auto& pdb : pdbs) {
            if (pdb->lookup_estimate(sample) > h_pattern) {
                ++count;
                break;
            }
        }
    }

    return count;
}

value_t MaximumCombinator::evaluate(
    const PPDBCollection& database,
    const State& state,
    value_t termination_cost)
{
    value_t best_estimate = -INFINITE_VALUE;

    for (const auto& pdb : database) {
        const value_t estimate = pdb->lookup_estimate(state);
        if (estimate == termination_cost) return estimate;
        best_estimate = std::max(estimate, best_estimate);
    }

    return best_estimate;
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd