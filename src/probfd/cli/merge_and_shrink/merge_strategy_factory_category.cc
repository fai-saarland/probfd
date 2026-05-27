#include "probfd/cli/merge_and_shrink/merge_strategy_factory_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/merge_and_shrink/merge_strategy_factory.h"

using namespace downward::cli::plugins;
using namespace probfd::merge_and_shrink;

namespace probfd::cli::merge_and_shrink {

void add_merge_strategy_factory_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_category_plugin<MergeStrategyFactory>(
        "PMergeStrategy",
        "This page describes the various merge strategies supported "
        "by the planner.");
}

} // namespace probfd::cli::merge_and_shrink
