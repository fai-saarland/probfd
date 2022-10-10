#include "max_orthogonal_finder.h"

#include "orthogonality.h"

#include "../../../../algorithms/max_cliques.h"

#include "../../../../option_parser.h"
#include "../../../../plugin.h"

#include "../../../../pdbs/pattern_cliques.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

MaxOrthogonalityFinder::MaxOrthogonalityFinder()
    : var_orthogonality(compute_prob_orthogonal_vars(false))
{
}

std::shared_ptr<std::vector<PatternSubCollection>>
MaxOrthogonalityFinder::compute_subcollections(
    const PatternCollection& patterns)
{
    std::vector<std::vector<int>> c_graph =
        build_compatibility_graph_orthogonality(patterns, var_orthogonality);

    std::shared_ptr<std::vector<PatternSubCollection>> additive_subcollections(
        new std::vector<PatternSubCollection>());

    max_cliques::compute_max_cliques(c_graph, *additive_subcollections);

    return additive_subcollections;
}

std::vector<PatternSubCollection>
MaxOrthogonalityFinder::compute_subcollections_with_pattern(
    const PatternCollection& patterns,
    const std::vector<PatternSubCollection>& known_pattern_cliques,
    const Pattern& new_pattern)
{
    return ::pdbs::compute_pattern_cliques_with_pattern(
        patterns,
        known_pattern_cliques,
        new_pattern,
        var_orthogonality);
}

static std::shared_ptr<MaxOrthogonalityFinder> _parse(OptionParser& parser)
{
    if (parser.dry_run()) return nullptr;

    return std::make_shared<MaxOrthogonalityFinder>();
}

static Plugin<SubCollectionFinder> _plugin("finder_max_orthogonality", _parse);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd