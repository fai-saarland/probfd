#include "downward/cli/plugins/plugin.h"

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
} _category_plugin;

} // namespace