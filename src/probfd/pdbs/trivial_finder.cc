#include "probfd/pdbs/trivial_finder.h"

#include <cassert>

namespace probfd::pdbs {

std::shared_ptr<std::vector<PatternSubCollection>>
TrivialFinder::compute_subcollections(const PatternCollection& patterns)
{
    std::shared_ptr<std::vector<PatternSubCollection>> additive_subcollections(
        new std::vector<PatternSubCollection>());
    additive_subcollections->reserve(patterns.size());

    int size = static_cast<int>(patterns.size());
    for (int i = 0; i < size; ++i) {
        additive_subcollections->push_back({i});
    }

    return additive_subcollections;
}

std::vector<PatternSubCollection>
TrivialFinder::compute_subcollections_with_pattern(
    const PatternCollection&,
    const std::vector<PatternSubCollection>&,
    const Pattern&)
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

} // namespace probfd::pdbs
