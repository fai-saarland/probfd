#include "probfd/heuristics/pdbs/trivial_finder.h"

#include "downward/plugins/plugin.h"
namespace probfd {
namespace heuristics {
namespace pdbs {

std::vector<PatternSubCollection>
TrivialFinder::compute_subcollections(const PPDBCollection& pdbs)
{
    std::vector<PatternSubCollection> additive_subcollections;
    additive_subcollections.reserve(pdbs.size());

    for (size_t i = 0; i < pdbs.size(); ++i) {
        additive_subcollections.push_back({static_cast<int>(i)});
    }

    return additive_subcollections;
}

std::vector<PatternSubCollection>
TrivialFinder::compute_subcollections_with_pdbs(
    const PPDBCollection&,
    const std::vector<PatternSubCollection>&,
    const ProbabilityAwarePatternDatabase&)
{
    return {};
}

value_t TrivialFinder::evaluate_subcollection(
    const std::vector<value_t>& pdb_estimates,
    const std::vector<int>& subcollection) const
{
    assert(subcollection.size() == 1);
    return pdb_estimates[subcollection.front()];
}

value_t TrivialFinder::combine(value_t, value_t) const
{
    // This should never be called.
    abort();
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd