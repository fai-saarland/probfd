#include "probfd/cli/solvers/task_solver_factory_category.h"

#include "language/plugins/registry.h"

#include "probfd/solver_interface.h"

using namespace language::plugins;

namespace probfd::cli::solvers {

void add_task_solver_factory_category(Namespace& nspace)
{
    nspace.insert_shared_type_declaration<TaskSolverFactory>(
        "TaskSolverFactory",
        "Represents a factory that produces a generic planning problem "
        "solver for a given probabilistic planning task. Expressions passed "
        "to the search command must produce values of this type.");
}

} // namespace probfd::cli::solvers