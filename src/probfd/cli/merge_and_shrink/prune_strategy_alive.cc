#include "probfd/cli/merge_and_shrink/prune_strategy_alive.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "probfd/merge_and_shrink/prune_strategy_alive.h"

using namespace probfd::merge_and_shrink;
using namespace downward;
using namespace downward::cli::plugins;

namespace {

InternalFunctionDefinitionBase& add_prune_strategy_alive_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "prune_alive",
        &downward::cli::plugins::
            construct_shared<PruneStrategy, PruneStrategyAlive>);

    f.document_title("Alive states prune strategy");
    f.document_synopsis("This prune strategy keeps only alive states.");

    return f;
}

} // namespace

namespace probfd::cli::merge_and_shrink {

void add_prune_strategy_alive_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_prune_strategy_alive_to_namespace(n);
}

} // namespace probfd::cli::merge_and_shrink