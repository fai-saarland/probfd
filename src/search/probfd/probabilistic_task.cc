#include "probfd/probabilistic_task.h"

#include "plugins/plugin.h"

namespace probfd {

static class ProbabilisticTaskCategoryPlugin
    : public plugins::TypedCategoryPlugin<ProbabilisticTask> {
public:
    ProbabilisticTaskCategoryPlugin()
        : TypedCategoryPlugin("ProbabilisticTask")
    {
        document_synopsis("Represents a probabilistic planning task");
    }
} _category_plugin_collection;

} // namespace probfd