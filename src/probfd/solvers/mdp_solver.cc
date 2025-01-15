#include "probfd/solvers/mdp_solver.h"

#include "probfd/tasks/root_task.h"

#include "probfd/caching_task_state_space.h"

#include "probfd/evaluator.h"
#include "probfd/interval.h"
#include "probfd/mdp_algorithm.h"
#include "probfd/policy.h"
#include "probfd/probabilistic_task.h"
#include "probfd/task_cost_function.h"
#include "probfd/task_evaluator_factory.h"
#include "probfd/task_state_space_factory.h"

#include "downward/utils/timer.h"

#include "downward/utils/exceptions.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <optional>
#include <type_traits>

namespace probfd::solvers {

MDPSolver::MDPSolver(
    utils::Verbosity verbosity,
    std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
    std::shared_ptr<TaskEvaluatorFactory> heuristic_factory,
    double max_time,
    std::string policy_filename,
    bool print_fact_names,
    std::optional<value_t> report_epsilon,
    bool report_enabled)
    : log_(utils::get_log_for_verbosity(verbosity))
    , task_state_space_factory_(std::move(task_state_space_factory))
    , heuristic_factory_(std::move(heuristic_factory))
    , max_time_(max_time)
    , policy_filename(std::move(policy_filename))
    , print_fact_names(print_fact_names)
    , report_epsilon(report_epsilon)
    , report_enabled(report_enabled)
{
}

MDPSolver::~MDPSolver() = default;

namespace {
class Printer {
    utils::Timer timer;
    std::ostream& out;

public:
    Printer(const std::string& print, std::ostream& out = std::cout)
        : out(out)
    {
        out << print << ' ' << std::flush;
    }

    ~Printer()
    {
        if (std::uncaught_exceptions() > 0) {
            out << "Failed after " << timer() << '.' << std::endl;
        } else {
            out << "Finished after " << timer() << '.' << std::endl;
        }
    }
};

template <typename F, typename... Args>
std::invoke_result_t<F, Args...>
timed(const std::string& print, F&& f, Args&&... args)
    requires std::invocable<F, Args...>
{
    Printer _(print, std::cout);
    return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
}

} // namespace

class Solver : public SolverInterface {
    std::shared_ptr<ProbabilisticTask> task;
    std::shared_ptr<FDRCostFunction> task_cost_function;

    std::unique_ptr<FDRMDPAlgorithm> algorithm;
    std::unique_ptr<TaskStateSpace> state_space;
    const std::shared_ptr<FDREvaluator> heuristic;
    std::string algorithm_name;
    double max_time;
    std::string policy_filename;
    bool print_fact_names;

    ProgressReport progress;

public:
    Solver(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        std::unique_ptr<FDRMDPAlgorithm> algorithm,
        std::unique_ptr<TaskStateSpace> state_space,
        const std::shared_ptr<FDREvaluator> heuristic,
        double max_time,
        std::string algorithm_name,
        std::string policy_filename,
        bool print_fact_names,
        std::optional<value_t> report_epsilon,
        bool report_enabled)
        : task(std::move(task))
        , task_cost_function(std::move(task_cost_function))
        , algorithm(std::move(algorithm))
        , state_space(std::move(state_space))
        , heuristic(std::move(heuristic))
        , algorithm_name(std::move(algorithm_name))
        , max_time(max_time)
        , policy_filename(std::move(policy_filename))
        , print_fact_names(print_fact_names)
        , progress(report_epsilon, std::cout, report_enabled)
    {
    }

    bool solve() override
    {
        std::cout << "Running MDP algorithm " << algorithm_name;

        if (max_time != std::numeric_limits<double>::infinity()) {
            std::cout << " with a time limit of " << max_time << " seconds.";
        }

        std::cout << " without a time limit." << std::endl;

        try {
            utils::Timer total_timer;

            const State& initial_state = state_space->get_initial_state();
            CompositeMDP<State, OperatorID> mdp{
                *state_space,
                *task_cost_function};

            std::unique_ptr<Policy<State, OperatorID>> policy =
                algorithm->compute_policy(
                    mdp,
                    *heuristic,
                    initial_state,
                    progress,
                    max_time);
            total_timer.stop();

            std::cout << "Finished after " << total_timer()
                      << " [t=" << utils::g_timer << "]" << std::endl;

            std::cout << std::endl;

            if (policy) {
                using namespace std;

                print_analysis_result(
                    policy->get_decision(initial_state)->q_value_interval);

                std::ofstream out(policy_filename);
                auto print_state = [this](
                                       const State& state,
                                       std::ostream& out) {
                    if (print_fact_names) {
                        out << state[0].get_name();
                        for (const FactProxy& fact : state | views::drop(1)) {
                            out << ", " << fact.get_name();
                        }
                    } else {
                        out << "{ " << state[0].get_variable().get_id()
                            << " -> " << state[0].get_value();

                        for (const FactProxy& fact : state | views::drop(1)) {
                            const auto [var, val] = fact.get_pair();
                            out << ", " << var << " -> " << val;
                        }
                        out << " }";
                    }
                };

                auto print_action =
                    [this](const OperatorID& op_id, std::ostream& out) {
                        out << this->task->get_operator_name(op_id.get_index());
                    };

                policy->print(out, print_state, print_action);
            }

            std::cout << std::endl;
            std::cout << "State space interface statistics:" << std::endl;
            state_space->print_statistics();

            std::cout << std::endl;
            std::cout << "Algorithm " << algorithm_name
                      << " statistics:" << std::endl;
            std::cout << "  Actual solver time: " << total_timer << std::endl;
            algorithm->print_statistics(std::cout);

            heuristic->print_statistics();

            return policy != nullptr;
        } catch (utils::TimeoutException&) {
            std::cout << "Time limit reached. Analysis was aborted."
                      << std::endl;
        }

        return false;
    }
};

std::unique_ptr<SolverInterface>
MDPSolver::create(const std::shared_ptr<ProbabilisticTask>& task)
{
    auto task_cost_function = std::make_shared<TaskCostFunction>(task);

    std::unique_ptr<FDRMDPAlgorithm> algorithm = timed(
        "Constructing algorithm...",
        &MDPSolver::create_algorithm,
        *this,
        task,
        task_cost_function);

    std::unique_ptr<TaskStateSpace> state_space = timed(
        "Constructing state space...",
        &TaskStateSpaceFactory::create_state_space,
        *task_state_space_factory_,
        task,
        task_cost_function);

    std::shared_ptr<FDREvaluator> heuristic = timed(
        "Constructing heuristic...",
        &TaskEvaluatorFactory::create_evaluator,
        *heuristic_factory_,
        task,
        task_cost_function);

    return std::make_unique<Solver>(
        task,
        std::move(task_cost_function),
        std::move(algorithm),
        std::move(state_space),
        std::move(heuristic),
        max_time_,
        get_algorithm_name(),
        policy_filename,
        print_fact_names,
        report_epsilon,
        report_enabled);
}

} // namespace probfd::solvers
