#include "downward/cli/plugins/plugin.h"

#include "probfd/task_evaluator_factory.h"

using namespace probfd;

using namespace downward::cli::plugins;

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