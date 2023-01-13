#include "probfd/heuristics/pdbs/trivial_finder.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

std::shared_ptr<std::vector<PatternSubCollection>>
TrivialFinder::compute_subcollections(const PatternCollection& patterns)
{
    std::shared_ptr<std::vector<PatternSubCollection>> additive_subcollections(
        new std::vector<PatternSubCollection>(patterns.size()));

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
    return {}; // There are no cliques.
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd