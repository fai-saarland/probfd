#include "probfd/cli/task_state_space_factory_category.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "probfd/task_state_space_factory.h"

using namespace language::plugins;

namespace probfd::cli {

void add_task_state_space_factory_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declaration<TaskStateSpaceFactory>(
        "TaskStateSpaceFactory",
        "");
}

} // namespace probfd::cli