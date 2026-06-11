#include "probfd_cli/heuristics/task_heuristic_factory_category.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "probfd/task_heuristic_factory.h"

using namespace language::plugins;

namespace probfd::cli::heuristics {

void add_task_heuristic_factory_category(RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugin<TaskHeuristicFactory>(
        "TaskHeuristicFactory");
}

} // namespace probfd::cli::heuristics