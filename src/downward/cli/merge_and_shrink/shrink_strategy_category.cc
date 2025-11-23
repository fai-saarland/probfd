#include "downward/cli/merge_and_shrink/shrink_strategy_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/merge_and_shrink/shrink_strategy.h"

using namespace downward::merge_and_shrink;

using namespace downward::cli::plugins;

namespace downward::cli::merge_and_shrink {

void add_shrink_strategy_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declaration<ShrinkStrategy>(
        "ShrinkStrategy",
        "This page describes the various shrink strategies supported "
        "by the planner.");
}

} // namespace downward::cli::merge_and_shrink
