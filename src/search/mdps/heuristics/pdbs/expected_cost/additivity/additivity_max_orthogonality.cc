#include "additivity_max_orthogonality.h"

#include "../../maxprob/orthogonality.h"

#include "../../../../../algorithms/max_cliques.h"

#include "../../../../../option_parser.h"
#include "../../../../../plugin.h"

namespace probabilistic {
namespace pdbs {
namespace additivity {

std::shared_ptr<std::vector<PatternClique>>
AdditivityMaxOrthogonality::compute_additive_subcollections(
    const PatternCollection& patterns)
{
    std::vector<std::vector<int>> c_graph =
        multiplicativity::build_compatibility_graph_orthogonality(patterns);

    std::shared_ptr<std::vector<PatternClique>> additive_subcollections(
        new std::vector<PatternClique>());

    max_cliques::compute_max_cliques(c_graph, *additive_subcollections);

    return additive_subcollections;
}

static std::shared_ptr<AdditivityMaxOrthogonality> _parse(OptionParser& parser)
{
    if (parser.dry_run()) return nullptr;

    return std::make_shared<AdditivityMaxOrthogonality>();
}

static Plugin<AdditivityStrategy> _plugin("max_orthogonality", _parse);

} // namespace additivity
} // namespace pdbs
} // namespace probabilistic