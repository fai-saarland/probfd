#include "probfd/cli/heuristics/task_heuristic_factory_category.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "probfd/task_heuristic_factory.h"

using namespace language::plugins;

namespace probfd::cli::heuristics {

void add_task_heuristic_factory_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declaration<TaskHeuristicFactory>(
        "TaskHeuristicFactory",
        "");
}

} // namespace probfd::cli::heuristics