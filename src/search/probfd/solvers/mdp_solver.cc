#include "probfd/solvers/mdp_solver.h"

#include "probfd/tasks/root_task.h"

#include "probfd/caching_task_state_space.h"

#include "probfd/evaluator.h"
#include "probfd/interval.h"
#include "probfd/mdp_algorithm.h"
#include "probfd/task_cost_function_factory.h"
#include "probfd/task_evaluator_factory.h"

#include "downward/utils/timer.h"

#include "downward/utils/exceptions.h"

#include "downward/plugins/options.h"
#include "downward/plugins/plugin.h"

#include <iostream>
#include <limits>
#include <optional>

class Evaluator;
class State;

namespace probfd {
class TaskCostFunctionFactory;
class TaskEvaluatorFactory;
} // namespace probfd

namespace probfd::solvers {

using namespace plugins;

MDPSolver::MDPSolver(const Options& opts)
    : task_(tasks::g_root_task)
    , task_proxy_(*task_)
    , task_cost_function_(
          opts.get<std::shared_ptr<TaskCostFunctionFactory>>("costs")
              ->create_cost_function(task_))
    , log_(utils::get_log_from_options(opts))
    , task_mdp_(
          opts.get<bool>("cache")
              ? new CachingTaskStateSpace(
                    task_,
                    log_,
                    task_cost_function_,
                    opts.get_list<std::shared_ptr<::Evaluator>>(
                        "path_dependent_evaluators"))
              : new TaskStateSpace(
                    task_,
                    log_,
                    task_cost_function_,
                    opts.get_list<std::shared_ptr<::Evaluator>>(
                        "path_dependent_evaluators")))
    , heuristic_(opts.get<std::shared_ptr<TaskEvaluatorFactory>>("eval")
                     ->create_evaluator(task_, task_cost_function_))
    , progress_(
          opts.contains("report_epsilon")
              ? std::optional<value_t>(opts.get<value_t>("report_epsilon"))
              : std::nullopt,
          std::cout,
          opts.get<bool>("report_enabled"))
    , max_time_(opts.get<double>("max_time"))
{
    progress_.register_print([&ss = *this->task_mdp_](std::ostream& out) {
        out << "registered=" << ss.get_num_registered_states();
    });
}

MDPSolver::~MDPSolver() = default;

void MDPSolver::solve()
{
    std::cout << "Running MDP algorithm " << get_algorithm_name();

    if (max_time_ != std::numeric_limits<double>::infinity()) {
        std::cout << " with a time limit of " << max_time_ << " seconds";
    }

    std::cout << "..." << std::endl;

    try {
        utils::Timer total_timer;
        std::unique_ptr<FDRMDPAlgorithm> algorithm = create_algorithm();

        const State& initial_state = task_mdp_->get_initial_state();

        Interval val = algorithm->solve(
            *task_mdp_,
            *heuristic_,
            initial_state,
            progress_,
            max_time_);
        total_timer.stop();

        std::cout << "analysis done. [t=" << utils::g_timer << "]" << std::endl;

        std::cout << std::endl;

        print_analysis_result(val);

        std::cout << std::endl;
        std::cout << "State space interface:" << std::endl;
        std::cout << "  Registered state(s): "
                  << task_mdp_->get_num_registered_states() << std::endl;
        task_mdp_->print_statistics();

        std::cout << std::endl;
        std::cout << "Algorithm " << get_algorithm_name()
                  << " statistics:" << std::endl;
        std::cout << "  Actual solver time: " << total_timer << std::endl;
        algorithm->print_statistics(std::cout);

        heuristic_->print_statistics();

        print_additional_statistics();
    } catch (utils::TimeoutException&) {
        std::cout << "Time limit reached. Analysis was aborted." << std::endl;
        solution_found_ = false;
    }
}

void MDPSolver::add_options_to_feature(Feature& feature)
{
    feature.add_option<std::shared_ptr<TaskCostFunctionFactory>>(
        "costs",
        "",
        "ssp()");
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
    utils::add_log_options_to_feature(feature);
}

} // namespace probfd::solvers
