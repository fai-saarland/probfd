#include "probfd/cli/pdbs/cegar/bfs_flaw_finder.h"

#include "language/ast/internal_function_definition.h"

#include "probfd/pdbs/cegar/bfs_flaw_finder.h"

using namespace downward;
using namespace probfd::pdbs::cegar;

using namespace language::parser;

namespace probfd::cli::pdbs::cegar {

InternalFunctionDefinitionBase&
add_bfs_flaw_finder_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<
        &construct_shared<FlawFindingStrategy, BFSFlawFinder, int>>(
        nspace,
        "bfs_flaw_finder");
    f.make_optional_argument_with_default(
        0,
        "max_search_states",
        "20M",
        "Maximal number of generated states after which the flaw search is "
        "aborted.");

    return f;
}

} // namespace probfd::cli::pdbs::cegar
