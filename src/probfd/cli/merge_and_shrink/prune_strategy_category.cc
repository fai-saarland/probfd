#include "probfd/cli/merge_and_shrink/prune_strategy_category.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "probfd/merge_and_shrink/prune_strategy.h"

using namespace probfd::merge_and_shrink;
using namespace language::plugins;

namespace probfd::cli::merge_and_shrink {

void add_prune_strategy_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declaration<PruneStrategy>(
        "PruneStrategy",
        "This page describes the various pruning strategies "
        "supported by the planner.");
}

} // namespace probfd::cli::merge_and_shrink
