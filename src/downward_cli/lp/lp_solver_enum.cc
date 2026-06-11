#include "downward_cli/lp/lp_solver_enum.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "downward/lp/lp_solver.h"

using namespace std;
using namespace downward::lp;

namespace downward::cli::lp {

void add_lp_solver_enum(language::plugins::RawRegistry& raw_registry)
{
    raw_registry.insert_enum_plugin<LPSolverType>(
        {{"cplex", "commercial solver by IBM"},
         {"soplex", "open source solver by ZIB"}});
}

} // namespace downward::cli::lp