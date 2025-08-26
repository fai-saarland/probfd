#include "probfd/cli/heuristics/task_heuristic_factory_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/task_heuristic_factory_category.h"

using namespace downward::cli::plugins;

namespace probfd::cli::heuristics {

void add_task_heuristic_factory_category(RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugin<TaskHeuristicFactory>(
        "TaskHeuristicFactory");
}

} // namespace probfd::cli::heuristics