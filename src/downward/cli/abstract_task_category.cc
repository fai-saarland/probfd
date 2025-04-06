#include "downward/cli/plugins/plugin.h"

#include "downward/abstract_task.h"

using namespace std;
using namespace downward::cli::plugins;

namespace {

class AbstractTaskCategoryPlugin
    : public TypedCategoryPlugin<downward::AbstractTask> {
public:
    AbstractTaskCategoryPlugin()
        : TypedCategoryPlugin("AbstractTask")
    {
        // TODO: Replace empty string by synopsis for the wiki page.
        document_synopsis("");
    }
};

AbstractTaskCategoryPlugin _category_plugin;

} // namespace