#include "probfd/cli/task_state_space_factory_category.h"

#include "language/ast/internal_type_declaration.h"

#include "probfd/task_state_space_factory.h"

using namespace language::parser;

namespace probfd::cli {

void add_task_state_space_factory_category(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<TaskStateSpaceFactory>(
        nspace,
        "TaskStateSpaceFactory",
        "");
}

} // namespace probfd::cli