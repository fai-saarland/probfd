#include "probfd/cli/solvers/task_solver_factory_category.h"

#include "language/ast/internal_type_declaration.h"

#include "probfd/solver_interface.h"

using namespace language::parser;

namespace probfd::cli::solvers {

void add_task_solver_factory_category(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<TaskSolverFactory>(
        nspace,
        "TaskSolverFactory",
        "Represents a factory that produces a generic planning problem "
        "solver for a given probabilistic planning task. Expressions passed "
        "to the search command must produce values of this type.");
}

} // namespace probfd::cli::solvers