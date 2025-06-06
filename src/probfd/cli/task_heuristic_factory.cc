#include "downward/cli/plugins/plugin.h"

#include "probfd/task_heuristic_factory.h"

using namespace probfd;

using namespace downward::cli::plugins;

namespace {

class TaskEvaluatorFactoryCategoryPlugin
    : public TypedCategoryPlugin<TaskHeuristicFactory> {
public:
    TaskEvaluatorFactoryCategoryPlugin()
        : TypedCategoryPlugin("TaskHeuristicFactory")
    {
        allow_variable_binding();
    }
} _category_plugin;

} // namespace