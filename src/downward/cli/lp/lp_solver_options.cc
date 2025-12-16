#include "downward/cli/lp/lp_solver_options.h"

#include "language/plugins/internal_function_definition.h"

using namespace std;

namespace downward::cli::lp {

std::size_t add_lp_solver_option_to_feature(
    language::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index)
{
    feature.make_optional_argument_with_default(
        start_index,
        "lpsolver",
        "cplex",
        "external solver that should be used to solve linear programs");

    feature.document_note(
        "Note",
        "to use an LP solver, you must build the planner with LP support. "
        "See [build instructions "
        "https://github.com/aibasel/downward/blob/main/BUILD.md].");

    return 1;
}

} // namespace downward::cli::lp