#include "downward_plugins/plugins/plugin.h"

#include "probfd/task_evaluator_factory.h"

using namespace probfd;

using namespace downward_plugins::plugins;

namespace {

class TaskEvaluatorFactoryCategoryPlugin
    : public TypedCategoryPlugin<TaskEvaluatorFactory> {
public:
    TaskEvaluatorFactoryCategoryPlugin()
        : TypedCategoryPlugin("TaskEvaluatorFactory")
    {
    }
} _category_plugin;

} // namespace