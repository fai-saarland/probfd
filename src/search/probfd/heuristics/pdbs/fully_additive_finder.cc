#include "probfd/heuristics/pdbs/fully_additive_finder.h"

#include "probfd/heuristics/pdbs/subcollections.h"

#include "downward/algorithms/max_cliques.h"

#include "downward/pdbs/pattern_cliques.h"

#include "downward/plugins/plugin.h"

#include <numeric>

namespace probfd {
namespace heuristics {
namespace pdbs {

std::vector<PatternSubCollection>
FullyAdditiveFinder::compute_subcollections(const PPDBCollection& pdbs)
{
    std::vector<PatternSubCollection> single_subcollection;
    auto& all_patterns = single_subcollection.emplace_back(pdbs.size());
    std::iota(all_patterns.begin(), all_patterns.end(), 0);
    return single_subcollection;
}

std::vector<PatternSubCollection>
FullyAdditiveFinder::compute_subcollections_with_pdbs(
    const PPDBCollection&,
    const std::vector<PatternSubCollection>&,
    const ProbabilityAwarePatternDatabase&)
{
    // This should never be needed
    abort();
}

value_t FullyAdditiveFinder::evaluate_subcollection(
    const std::vector<value_t>& pdb_estimates,
    const std::vector<int>& subcollection) const
{
    auto result = 0_vt;

    for (int pattern_id : subcollection) {
        result += pdb_estimates[pattern_id];
    }

    return result;
}

value_t FullyAdditiveFinder::combine(value_t left, value_t right) const
{
    return left + right;
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd