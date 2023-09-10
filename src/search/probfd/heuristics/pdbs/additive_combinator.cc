#include "probfd/heuristics/pdbs/additive_combinator.h"

#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"

#include <numeric>

namespace probfd {
namespace heuristics {
namespace pdbs {

value_t AdditiveCombinator::evaluate(
    const PPDBCollection& pdbs,
    const State& state,
    value_t termination_cost)
{
    auto result = 0_vt;

    for (const auto& pdb : pdbs) {
        const auto estimate = pdb->lookup_estimate(state);
        if (estimate == termination_cost) return estimate;
        result += estimate;
    }

    return result;
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd