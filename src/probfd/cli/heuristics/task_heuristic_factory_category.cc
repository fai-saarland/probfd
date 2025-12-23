#include "probfd/cli/heuristics/task_heuristic_factory_category.h"

#include "language/plugins/registry.h"

#include "probfd/task_heuristic_factory.h"

using namespace language::plugins;

namespace probfd::cli::heuristics {

void add_task_heuristic_factory_category(Namespace& nspace)
{
    nspace.insert_shared_type_declaration<TaskHeuristicFactory>(
        "TaskHeuristicFactory",
        "");
}

} // namespace probfd::cli::heuristics