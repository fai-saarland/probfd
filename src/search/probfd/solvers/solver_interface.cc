#include "probfd/solvers/solver_interface.h"

#include "plugin.h"

namespace probfd {

static PluginTypePlugin<SolverInterface> _type_plugin(
    "SolverInterface",
    // TODO: Replace empty string by synopsis for the wiki page.
    "");

} // namespace probfd