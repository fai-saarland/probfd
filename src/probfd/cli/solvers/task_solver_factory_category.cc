#include "probfd/cli/solvers/task_solver_factory_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/solver_interface.h"

using namespace downward::cli::plugins;

namespace probfd::cli::solvers {

void add_task_solver_factory_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_category_plugin<TaskSolverFactory>(
        "TaskSolverFactory",
        "Represents a factory that produces a generic planning problem "
        "solver for a given probabilistic planning task. Expressions passed "
        "to the search command must produce values of this type.");
}

} // namespace probfd::cli::solvers