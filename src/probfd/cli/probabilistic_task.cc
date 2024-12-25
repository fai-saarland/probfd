#include "downward/cli/plugins/plugin.h"

#include "probfd/probabilistic_task.h"

using namespace probfd;

using namespace downward::cli::plugins;

namespace {

class ProbabilisticTaskCategoryPlugin
    : public TypedCategoryPlugin<ProbabilisticTask> {
public:
    ProbabilisticTaskCategoryPlugin()
        : TypedCategoryPlugin("ProbabilisticTask")
    {
        document_synopsis("Represents a probabilistic planning task");
    }
} _category_plugin_collection;

} // namespace