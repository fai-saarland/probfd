#include "probfd/cli/solvers/task_solver_factory_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/solver_interface.h"

using namespace downward::cli::plugins;

namespace probfd::cli::solvers {

void add_task_solver_factory_category(RawRegistry& raw_registry)
{
    auto& category = raw_registry.insert_category_plugin<TaskSolverFactory>(
        "TaskSolverFactory");
    category.document_synopsis(
        "Represents a factory that produces a generic planning problem "
        "solver for a given probabilistic planning task.");
}

} // namespace probfd::cli::solvers