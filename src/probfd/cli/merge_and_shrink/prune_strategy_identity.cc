#include "probfd/cli/merge_and_shrink/prune_strategy_identity.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/merge_and_shrink/prune_strategy_identity.h"

using namespace probfd::merge_and_shrink;
using namespace downward;
using namespace language::plugins;

namespace {

InternalFunctionDefinitionBase& add_prune_strategy_identity_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "prune_identity",
        &language::plugins::
            construct_shared<PruneStrategy, PruneStrategyIdentity>);

    f.document_title("Identity prune strategy");
    f.document_synopsis("This prune strategy leaves the TS unchanged.");

    return f;
}

} // namespace

namespace probfd::cli::merge_and_shrink {

void add_prune_strategy_identity_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_prune_strategy_identity_to_namespace(n);
}

} // namespace probfd::cli::merge_and_shrink