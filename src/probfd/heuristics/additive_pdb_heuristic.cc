#include "probfd/heuristics/additive_pdb_heuristic.h"

#include "probfd/pdbs/probability_aware_pattern_database.h"

#include "probfd/value_type.h"

#include <algorithm>
#include <utility>

using namespace downward;
using namespace probfd::pdbs;

namespace probfd::heuristics {

namespace {
bool is_trivial(const ProbabilityAwarePatternDatabase& pdb)
{
    return std::ranges::all_of(pdb.value_table, [](value_t v) {
        return v == 0_vt;
    });
}
} // namespace

AdditivePDBHeuristic::AdditivePDBHeuristic(
    std::vector<ProbabilityAwarePatternDatabase> pdbs)
    : pdbs_(std::move(pdbs))
{
    std::erase_if(pdbs_, is_trivial);
}

AdditivePDBHeuristic::~AdditivePDBHeuristic() = default;

value_t AdditivePDBHeuristic::evaluate(const State& state) const
{
    value_t value = 0.0_vt;

    for (const auto& pdb : pdbs_) {
        const value_t estimate = pdb.lookup_estimate(state);

        if (estimate == INFINITE_VALUE) { return estimate; }

        value += estimate;
    }

    return value;
}

} // namespace probfd::heuristics
