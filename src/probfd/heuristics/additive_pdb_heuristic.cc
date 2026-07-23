#include "probfd/heuristics/additive_pdb_heuristic.h"

#include "probfd/pdbs/probability_aware_pattern_database.h"

#include "probfd/value_type.h"

#include <utility>

using namespace downward;
using namespace probfd::pdbs;

namespace probfd::heuristics {

AdditivePDBHeuristic::AdditivePDBHeuristic(
    std::vector<ProbabilityAwarePatternDatabase> pdbs)
    : pdbs_(std::move(pdbs))
{
}

AdditivePDBHeuristic::~AdditivePDBHeuristic() = default;

value_t AdditivePDBHeuristic::evaluate(const State& state) const
{
    value_t value = 0_vt;

    for (const auto& pdb : pdbs_) {
        const value_t estimate = pdb.lookup_estimate(state);

        if (estimate == INFINITE_VALUE) {
            return INFINITE_VALUE;
        }

        value += estimate;
    }

    return value;
}

} // namespace probfd::heuristics
