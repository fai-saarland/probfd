#include "downward/cli/plugins/plugin.h"

#include "downward/cli/utils/logging_options.h"

#include "probfd/cli/solvers/mdp_solver_options.h"

using namespace probfd;

using namespace downward::cli::plugins;

using downward::cli::utils::add_log_options_to_feature;

namespace probfd::cli::solvers {

std::size_t
add_base_solver_options_to_feature(InternalFunctionDefinitionBase& feature, std::size_t start_index)
{
    feature.make_required_argument(
        start_index,
        "algorithm",
        "The algorithm to be used by the search.");
    const auto n = add_base_solver_options_except_algorithm_to_feature(
        feature,
        start_index + 1);

    return n + 1;
}

std::size_t add_base_solver_options_except_algorithm_to_feature(
    InternalFunctionDefinitionBase& feature,
    std::size_t start_index)
{
    feature.make_optional_argument_with_default(
        start_index,
        "state_space",
        "default_state_space()",
        "The factory implementation for the task state space.");
    feature.make_optional_argument_with_default(
        start_index + 1,
        "heuristic",
        "blind_heuristic()",
        "The heuristic to be used by the search.");
    feature.make_optional_argument_with_default(
        start_index + 2,
        "policy_file",
        "\"\"",
        "Name of the file in which the policy returned by the algorithm is "
        "written. If empty, policy is not written.");
    feature.make_optional_argument_with_default(
        start_index + 3,
        "print_fact_names",
        "true",
        "Determines the way in which state facts are written to the policy "
        "output file. If true, they are printed using the fact names of the "
        "probabilistic SAS input file. Otherwise, each fact is printed as v -> "
        "d, where v is the index of the variable of the fact and d is the "
        "index of the value of the fact.");
    feature.make_optional_argument_with_default(
        start_index + 4,
        "report_epsilon",
        "1e-4",
        "Algorithms will report the current initial state objective value to "
        "stdout if it changed by more than this threshold.");
    feature.make_optional_argument_with_default(
        start_index + 5,
        "report_enabled",
        "true",
        "Whether the algorithm should report its progress.");
    const auto n = add_log_options_to_feature(feature, start_index + 6);
    return n + 6;
}

} // namespace probfd::cli::solvers
