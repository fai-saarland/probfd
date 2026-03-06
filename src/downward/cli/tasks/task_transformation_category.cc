#include "downward/cli/tasks/task_transformation_category.h"

#include "downward/task_transformation.h"

#include "language/ast/internal_type_declaration.h"

using namespace language::parser;

namespace downward::cli::tasks {

void add_task_transformation_category(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<TaskTransformation>(
        nspace,
        "TaskTransformation",
        "");
}

} // namespace downward::cli::tasks