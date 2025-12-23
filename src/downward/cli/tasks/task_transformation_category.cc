#include "downward/cli/tasks/task_transformation_category.h"

#include "language/plugins/registry.h"

#include "downward/task_transformation.h"

using namespace language::plugins;

namespace downward::cli::tasks {

void add_task_transformation_category(Namespace& nspace)
{
    nspace.insert_shared_type_declaration<TaskTransformation>(
        "TaskTransformation",
        "");
}

} // namespace downward::cli::tasks