#include "probfd/cli/merge_and_shrink/prune_strategy_solvable.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/merge_and_shrink/prune_strategy_solvable.h"

using namespace probfd::merge_and_shrink;
using namespace downward;
using namespace language::plugins;

namespace {

InternalFunctionDefinitionBase& add_prune_strategy_solvable_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "prune_solvable",
        &language::plugins::
            construct_shared<PruneStrategy, PruneStrategySolvable>);

    f.document_title("Solvable states prune strategy");
    f.document_synopsis("This prune strategy keeps only solvable states.");

    return f;
}

} // namespace

namespace probfd::cli::merge_and_shrink {

void add_prune_strategy_solvable_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_prune_strategy_solvable_to_namespace(n);
}

} // namespace probfd::cli::merge_and_shrink