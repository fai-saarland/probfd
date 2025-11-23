#include "probfd/cli/merge_and_shrink/shrink_strategy_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/merge_and_shrink/shrink_strategy.h"

using namespace downward::cli::plugins;
using namespace probfd::merge_and_shrink;

namespace probfd::cli::merge_and_shrink {

void add_shrink_strategy_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declaration<ShrinkStrategy>(
        "PShrinkStrategy",
        "This page describes the various shrink strategies supported "
        "by the planner.");
}

} // namespace probfd::cli::merge_and_shrink
