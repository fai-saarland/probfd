#include "probfd/task_types.h"

#include "probfd/evaluator.h"

#include "downward/plugins/plugin.h"

namespace probfd {

static class TaskEvaluatorCategoryPlugin
    : public plugins::TypedCategoryPlugin<TaskEvaluator> {
public:
    TaskEvaluatorCategoryPlugin()
        : TypedCategoryPlugin("TaskEvaluator")
    {
    }
} _category_plugin_evaluator;

} // namespace probfd
