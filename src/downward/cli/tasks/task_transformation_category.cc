#include "downward/cli/tasks/task_transformation_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/task_transformation.h"

using namespace downward::cli::plugins;

namespace downward::cli::tasks {

void add_task_transformation_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declaration<TaskTransformation>(
        "TaskTransformation",
        "");
}

} // namespace downward::cli::tasks