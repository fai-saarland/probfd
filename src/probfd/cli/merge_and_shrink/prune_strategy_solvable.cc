#include "probfd/cli/merge_and_shrink/prune_strategy_solvable.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/merge_and_shrink/prune_strategy_solvable.h"

using namespace probfd::merge_and_shrink;
using namespace downward;
using namespace language::plugins;

namespace probfd::cli::merge_and_shrink {

InternalFunctionDefinitionBase&
add_prune_strategy_solvable_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "prune_solvable",
        &language::plugins::
            construct_shared<PruneStrategy, PruneStrategySolvable>);

    f.document_title("Solvable states prune strategy");
    f.document_synopsis("This prune strategy keeps only solvable states.");

    return f;
}

} // namespace probfd::cli::merge_and_shrink