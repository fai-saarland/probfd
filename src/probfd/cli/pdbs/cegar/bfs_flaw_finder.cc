#include "probfd/cli/pdbs/cegar/bfs_flaw_finder.h"

#include "language/plugins/registry.h"

#include "language/plugins/internal_function_definition.h"

#include "probfd/pdbs/cegar/bfs_flaw_finder.h"

using namespace downward;
using namespace probfd::pdbs::cegar;

using namespace language::plugins;

namespace probfd::cli::pdbs::cegar {

InternalFunctionDefinitionBase& add_bfs_flaw_finder_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "bfs_flaw_finder",
        &construct_shared<FlawFindingStrategy, BFSFlawFinder, int>);
    f.make_optional_argument_with_default(
        0,
        "max_search_states",
        "20M",
        "Maximal number of generated states after which the flaw search is "
        "aborted.");

    return f;
}

} // namespace probfd::cli::pdbs::cegar
