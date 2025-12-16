#include "downward/cli/lp/lp_solver_enum.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/lp/lp_solver.h"

using namespace std;
using namespace downward::lp;

namespace downward::cli::lp {

void add_lp_solver_enum(language::plugins::Registry& registry)
{
    language::plugins::Namespace& n = registry.get_global_name_space();
    n.insert_enum_declaration<LPSolverType>(
        {{"cplex", "commercial solver by IBM"},
         {"soplex", "open source solver by ZIB"}});
}

} // namespace downward::cli::lp