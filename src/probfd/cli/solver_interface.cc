#include "downward/cli/plugins/plugin.h"

#include "probfd/solver_interface.h"

using namespace probfd;

using namespace downward::cli::plugins;

namespace {

class TaskSolverFactoryCategoryPlugin
    : public TypedCategoryPlugin<TaskSolverFactory> {
public:
    TaskSolverFactoryCategoryPlugin()
        : TypedCategoryPlugin("TaskSolverFactory")
    {
        document_synopsis(
            "Represents a factory that produces a generic planning problem "
            "solver for a given probabilistic planning task.");
    }
} _category_plugin;

} // namespace