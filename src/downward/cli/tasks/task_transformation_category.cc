#include "downward/cli/tasks/task_transformation_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/task_transformation.h"

using namespace downward::cli::plugins;

namespace downward::cli::tasks {

void add_task_transformation_category(RawRegistry& raw_registry)
{
    auto& category = raw_registry.insert_category_plugin<TaskTransformation>("TaskTransformation");

    // TODO: Replace empty string by synopsis for the wiki page.
    category.document_synopsis("");
}

} // namespace