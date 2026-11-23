#include "probfd/heuristics/additive_pdb_heuristic.h"

#include "probfd/pdbs/probability_aware_pattern_database.h"

#include "probfd/heuristics/utils.h"

#include "downward/views/transform.h"

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
    const auto get_estimate = std::bind_back(
        &ProbabilityAwarePatternDatabase::lookup_estimate,
        std::cref(state));

    return sum_estimate(pdbs_ | std::views::transform(get_estimate));
}

} // namespace probfd::heuristics
