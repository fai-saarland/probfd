#include "probfd/cli/solvers/task_solver_factory_category.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/probabilistic_task_solver.h"

using namespace language::plugins;

namespace probfd::cli::solvers {

void add_task_solver_factory_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declaration<ProbabilisticTaskSolver>(
        "SolverInterface",
        "Represents a generic solver for a given probabilistic planning task. "
        "Expressions passed to the search command must produce values of this "
        "type.");
}

} // namespace probfd::cli::solvers