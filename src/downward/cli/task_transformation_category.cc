#include "downward/cli/plugins/plugin.h"

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

TaskTransformationCategoryPlugin _category_plugin;

} // namespace