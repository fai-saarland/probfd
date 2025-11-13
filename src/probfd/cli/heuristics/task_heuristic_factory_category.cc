#include "probfd/cli/heuristics/task_heuristic_factory_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/task_heuristic_factory.h"

using namespace downward::cli::plugins;

namespace probfd::cli::heuristics {

void add_task_heuristic_factory_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_category_plugin<TaskHeuristicFactory>(
        "TaskHeuristicFactory",
        "");
}

} // namespace probfd::cli::heuristics