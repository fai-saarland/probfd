#include "probfd/cli/merge_and_shrink/prune_strategy_alive.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/merge_and_shrink/prune_strategy_alive.h"

using namespace probfd::merge_and_shrink;
using namespace downward;
using namespace downward::cli::plugins;

namespace {

Feature& add_prune_strategy_alive_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "prune_alive",
        &downward::cli::plugins::
            make_shared<PruneStrategy, PruneStrategyAlive>);

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