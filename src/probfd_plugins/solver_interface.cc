#include "probfd/solver_interface.h"

#include "downward/plugins/plugin.h"

using namespace probfd;

namespace {

class SolverInterfaceCategoryPlugin
    : public plugins::TypedCategoryPlugin<SolverInterface> {
public:
    SolverInterfaceCategoryPlugin()
        : TypedCategoryPlugin("SolverInterface")
    {
        document_synopsis("Represents a generic planning problem solver");
    }
} _category_plugin;

} // namespace