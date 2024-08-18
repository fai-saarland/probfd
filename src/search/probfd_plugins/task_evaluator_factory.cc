#include "probfd/task_evaluator_factory.h"

#include "downward/plugins/plugin.h"

using namespace probfd;

namespace {

class TaskEvaluatorFactoryCategoryPlugin
    : public plugins::TypedCategoryPlugin<TaskEvaluatorFactory> {
public:
    TaskEvaluatorFactoryCategoryPlugin()
        : TypedCategoryPlugin("TaskEvaluatorFactory")
    {
    }
} _category_plugin;

} // namespace