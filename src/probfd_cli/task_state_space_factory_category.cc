#include "probfd_cli/task_state_space_factory_category.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "probfd/task_state_space_factory.h"

using namespace language::plugins;

namespace probfd::cli {

void add_task_state_space_factory_category(RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugin<TaskStateSpaceFactory>(
        "TaskStateSpaceFactory");
}

} // namespace probfd::cli