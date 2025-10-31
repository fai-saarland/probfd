#include "downward/cli/lp/lp_solver_options.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/lp/lp_solver.h"

using namespace std;
using namespace downward::lp;

namespace downward::cli::lp {

void add_lp_solver_option_to_feature(plugins::Feature& feature)
{
    feature.add_optional_argument_with_default<LPSolverType>(
        "lpsolver",
        "cplex",
        "external solver that should be used to solve linear programs");

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

} // namespace downward::cli::lp