#include "downward/cli/tasks/task_transformation_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/task_transformation.h"

using namespace downward::cli::plugins;

namespace downward::cli::tasks {

void add_task_transformation_category(Registry& raw_registry)
{
    raw_registry.insert_shared_category_plugin<TaskTransformation>(
        "TaskTransformation",
        "");
}

} // namespace downward::cli::tasks