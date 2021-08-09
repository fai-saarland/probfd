#include "multiplicativity_max_orthogonality.h"

#include "../../orthogonality.h"

#include "../../../../../algorithms/max_cliques.h"

#include "../../../../../option_parser.h"
#include "../../../../../plugin.h"

namespace probabilistic {
namespace pdbs {
namespace maxprob {
namespace multiplicativity {

std::shared_ptr<std::vector<PatternClique>>
MultiplicativityMaxOrthogonality::compute_multiplicative_subcollections(
    const PatternCollection& patterns)
{
    std::vector<std::vector<int>> c_graph =
        build_compatibility_graph_orthogonality(patterns);

    std::shared_ptr<std::vector<PatternClique>> subcollections(
        new std::vector<PatternClique>());

    max_cliques::compute_max_cliques(c_graph, *subcollections);

    return subcollections;
}

static std::shared_ptr<MultiplicativityMaxOrthogonality>
_parse(OptionParser& parser)
{
    if (parser.dry_run()) return nullptr;

    return std::make_shared<MultiplicativityMaxOrthogonality>();
}

static Plugin<MultiplicativityStrategy>
    _plugin("multiplicativity_orthogonality", _parse);

} // namespace multiplicativity
} // namespace maxprob
} // namespace pdbs
} // namespace probabilistic