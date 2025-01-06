#include "downward/cli/lp/lp_solver_options.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/lp/lp_solver.h"

using namespace std;
using namespace lp;

namespace downward::cli::lp {

void add_lp_solver_option_to_feature(plugins::Feature& feature)
{
    feature.add_option<LPSolverType>(
        "lpsolver",
        "external solver that should be used to solve linear programs",
        "cplex");

    feature.document_note(
        "Note",
        "to use an LP solver, you must build the planner with LP support. "
        "See [build instructions "
        "https://github.com/aibasel/downward/blob/main/BUILD.md].");
}

tuple<LPSolverType>
get_lp_solver_arguments_from_options(const plugins::Options& opts)
{
    return make_tuple(opts.get<LPSolverType>("lpsolver"));
}

plugins::TypedEnumPlugin<LPSolverType> _enum_plugin(
    {{"cplex", "commercial solver by IBM"},
     {"soplex", "open source solver by ZIB"}});

} // namespace downward::cli::lp