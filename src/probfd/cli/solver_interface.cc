#include "downward/cli/plugins/plugin.h"

#include "probfd/solver_interface.h"

using namespace probfd;

using namespace downward::cli::plugins;

namespace {

class SolverInterfaceCategoryPlugin
    : public TypedCategoryPlugin<SolverInterface> {
public:
    SolverInterfaceCategoryPlugin()
        : TypedCategoryPlugin("SolverInterface")
    {
        document_synopsis("Represents a generic planning problem solver");
    }
} _category_plugin;

} // namespace