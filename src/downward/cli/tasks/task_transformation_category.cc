#include "downward/cli/tasks/task_transformation_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/task_transformation.h"

using namespace std;
using namespace downward::cli::plugins;

namespace {
class TaskTransformationCategoryPlugin
    : public TypedCategoryPlugin<downward::TaskTransformation> {
public:
    TaskTransformationCategoryPlugin()
        : TypedCategoryPlugin("TaskTransformation")
    {
        // TODO: Replace empty string by synopsis for the wiki page.
        document_synopsis("");
    }
};
}

namespace downward::cli::tasks {

void add_task_transformation_category(RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugin<TaskTransformationCategoryPlugin>();
}

} // namespace