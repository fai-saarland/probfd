#include "probfd/probabilistic_task.h"

#include "downward/plugins/plugin.h"

using namespace plugins;
using namespace probfd;

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