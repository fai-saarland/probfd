#include "downward/cli/merge_and_shrink/merge_strategy_factory_category.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/merge_and_shrink/merge_strategy_factory.h"

using namespace downward::merge_and_shrink;

using namespace language::plugins;

namespace downward::cli::merge_and_shrink {

void add_merge_strategy_factory_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declaration<MergeStrategyFactory>(
        "MergeStrategy",
        "This page describes the various merge strategies supported "
        "by the planner.");
}

} // namespace downward::cli::merge_and_shrink
