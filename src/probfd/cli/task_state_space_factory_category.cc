#include "probfd/cli/task_state_space_factory_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/task_state_space_factory.h"

using namespace downward::cli::plugins;

namespace probfd::cli {

void add_task_state_space_factory_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_category_plugin<TaskStateSpaceFactory>(
        "TaskStateSpaceFactory",
        "");
}

} // namespace probfd::cli