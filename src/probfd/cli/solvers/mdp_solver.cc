#include "downward/cli/plugins/plugin.h"

#include "downward/cli/utils/logging_options.h"

#include "probfd/cli/solvers/mdp_solver.h"

#include <optional>

using namespace probfd;
using namespace probfd::solvers;

using namespace downward::cli::plugins;

using downward::cli::utils::add_log_options_to_feature;
using downward::cli::utils::get_log_arguments_from_options;

namespace probfd::cli::solvers {

void add_base_solver_options_to_feature(Feature& feature)
{
    feature.add_option<std::shared_ptr<StatisticalMDPAlgorithmFactory>>(
        "algorithm",
        "The algorithm to be used by the search.",
        ArgumentInfo::NO_DEFAULT);
    add_base_solver_options_except_algorithm_to_feature(feature);
}

void add_base_solver_options_except_algorithm_to_feature(Feature& feature)
{
    feature.add_option<std::shared_ptr<TaskStateSpaceFactory>>(
        "state_space",
        "The factory implementation for the task state space.",
        "default_state_space()");
    feature.add_option<std::shared_ptr<TaskHeuristicFactory>>(
        "heuristic",
        "The heuristic to be used by the search.",
        "blind_heuristic()");
    feature.add_option<value_t>(
        "report_epsilon",
        "Algorithms will report the current initial state objective value to "
        "stdout if it changed by more than this threshold.",
        "1e-4");
    feature.add_option<bool>(
        "report_enabled",
        "Whether the algorithm should report its progress.",
        "true");
    feature.add_option<std::string>(
        "policy_file",
        "Name of the file in which the policy returned by the algorithm is "
        "written.",
        "\"sas_policy\"");
    feature.add_option<bool>(
        "print_fact_names",
        "Determines the way in which state facts are written to the policy "
        "output file. If true, they are printed using the fact names of the "
        "probabilistic SAS input file. Otherwise, each fact is printed as v -> "
        "d, where v is the index of the variable of the fact and d is the "
        "index of the value of the fact.",
        "true");
    add_log_options_to_feature(feature);
}

MDPSolverArgs get_base_solver_args_from_options(const Options& options)
{
    return std::tuple_cat(
        std::make_tuple(
            options.get<std::shared_ptr<StatisticalMDPAlgorithmFactory>>(
                "algorithm"),
            options.get<std::shared_ptr<TaskStateSpaceFactory>>("state_space"),
            options.get<std::shared_ptr<TaskHeuristicFactory>>("heuristic")),
        get_log_arguments_from_options(options),
        std::make_tuple(
            options.get<std::string>("policy_file"),
            options.get<bool>("print_fact_names"),
            options.contains("report_epsilon")
                ? std::optional<value_t>(options.get<value_t>("report_epsilon"))
                : std::nullopt,
            options.get<bool>("report_enabled")));
}

MDPSolverNoAlgorithmArgs
get_base_solver_args_no_algorithm_from_options(const Options& options)
{
    return std::tuple_cat(
        std::make_tuple(
            options.get<std::shared_ptr<TaskStateSpaceFactory>>("state_space"),
            options.get<std::shared_ptr<TaskHeuristicFactory>>("heuristic")),
        get_log_arguments_from_options(options),
        std::make_tuple(
            options.get<std::string>("policy_file"),
            options.get<bool>("print_fact_names"),
            options.contains("report_epsilon")
                ? std::optional<value_t>(options.get<value_t>("report_epsilon"))
                : std::nullopt,
            options.get<bool>("report_enabled")));
}

} // namespace probfd::cli::solvers
