#include "probfd/task_evaluator_factory.h"

#include "downward/plugins/plugin.h"

namespace probfd {

static class TaskEvaluatorFactoryCategoryPlugin
    : public plugins::TypedCategoryPlugin<TaskEvaluatorFactory> {
public:
    TaskEvaluatorFactoryCategoryPlugin()
        : TypedCategoryPlugin("TaskEvaluatorFactory")
    {
    }
} _category_plugin;

} // namespace probfd