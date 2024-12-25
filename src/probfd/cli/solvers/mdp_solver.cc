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
        "",
        "blind_eval()");
    feature.add_option<bool>("cache", "", "false");
    feature.add_list_option<std::shared_ptr<::Evaluator>>(
        "path_dependent_evaluators",
        "",
        "[]");
    feature.add_option<value_t>("report_epsilon", "", "1e-4");
    feature.add_option<bool>("report_enabled", "", "true");
    feature.add_option<double>("max_time", "", "infinity");
    feature.add_option<std::string>("policy_file", "", "\"sas_policy\"");
    feature.add_option<bool>("print_fact_names", "", "true");
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
