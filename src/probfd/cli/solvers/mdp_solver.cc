#include "downward/cli/plugins/plugin.h"

#include "downward/cli/utils/logging_options.h"

#include "probfd/cli/solvers/mdp_solver.h"

#include <optional>

using namespace probfd;

using namespace downward::cli::plugins;

using downward::cli::utils::add_log_options_to_feature;
using downward::cli::utils::get_log_arguments_from_options;

namespace probfd::cli::solvers {

void add_base_solver_options_to_feature(Feature& feature)
{
    feature.add_option<std::shared_ptr<TaskEvaluatorFactory>>(
        "eval",
        "The heuristic to be used by the search.",
        "blind_eval()");
    feature.add_option<bool>(
        "cache",
        "Whether the state space should be cached to avoid re-generating "
        "transitions. May drastically increase memory footprint.",
        "false");
    feature.add_list_option<std::shared_ptr<::Evaluator>>(
        "path_dependent_evaluators",
        "A list of path-dependent classical planning evaluators to inform of "
        "new transitions during the search.",
        "[]");
    feature.add_option<value_t>(
        "report_epsilon",
        "Algorithms will report the current initial state objective value to "
        "stdout if it changed by more than this threshold.",
        "1e-4");
    feature.add_option<bool>(
        "report_enabled",
        "Whether the algorithm should report its progress.",
        "true");
    feature.add_option<double>(
        "max_time",
        "The time limit after which the algorithms aborts with an exception. "
        "This limit is not strict, algorithms only periodically check that "
        "they "
        "have no exhausted the time limit.",
        "infinity");
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

std::tuple<
    utils::Verbosity,
    std::vector<std::shared_ptr<::Evaluator>>,
    bool,
    std::shared_ptr<TaskEvaluatorFactory>,
    std::optional<value_t>,
    bool,
    double,
    std::string,
    bool>
get_base_solver_args_from_options(const Options& options)
{
    return std::tuple_cat(
        get_log_arguments_from_options(options),
        std::make_tuple(
            options.get_list<std::shared_ptr<::Evaluator>>(
                "path_dependent_evaluators"),
            options.get<bool>("cache"),
            options.get<std::shared_ptr<TaskEvaluatorFactory>>("eval"),
            options.contains("report_epsilon")
                ? std::optional<value_t>(options.get<value_t>("report_epsilon"))
                : std::nullopt,
            options.get<bool>("report_enabled"),
            options.get<double>("max_time"),
            options.get<std::string>("policy_file"),
            options.get<bool>("print_fact_names")));
}

} // namespace probfd::cli::solvers
