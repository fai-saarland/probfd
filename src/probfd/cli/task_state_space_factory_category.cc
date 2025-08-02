#include "probfd/cli/task_state_space_factory_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/task_state_space_factory.h"

using namespace probfd;

using namespace downward::cli::plugins;

namespace {
class TaskStateSpaceFactoryCategoryPlugin
    : public TypedCategoryPlugin<TaskStateSpaceFactory> {
public:
    TaskStateSpaceFactoryCategoryPlugin()
        : TypedCategoryPlugin("TaskStateSpaceFactory")
    {
    }
};
}

namespace probfd::cli {

void add_task_state_space_factory_category(RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugin<TaskStateSpaceFactoryCategoryPlugin>();
}

} // namespace