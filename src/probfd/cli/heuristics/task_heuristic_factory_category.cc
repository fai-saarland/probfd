#include "probfd/cli/heuristics/task_heuristic_factory_category.h"

#include "language/ast/internal_type_declaration.h"

#include "probfd/task_heuristic_factory.h"

using namespace language::parser;

namespace probfd::cli::heuristics {

void add_task_heuristic_factory_category(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<TaskHeuristicFactory>(
        nspace,
        "TaskHeuristicFactory",
        "");
}

} // namespace probfd::cli::heuristics