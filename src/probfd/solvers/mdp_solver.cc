#include "probfd/solvers/mdp_solver.h"

#include "probfd/solvers/statistical_mdp_algorithm.h"

#include "probfd/caching_task_state_space.h"

#include "probfd/heuristic.h"
#include "probfd/mdp_algorithm.h"
#include "probfd/policy.h"
#include "probfd/probabilistic_task.h"
#include "probfd/task_cost_function.h"
#include "probfd/task_heuristic_factory.h"
#include "probfd/task_state_space_factory.h"
#include "probfd/transition_tail.h"

#include "probfd/utils/timed.h"

#include "downward/utils/timer.h"

#include "downward/utils/exceptions.h"

#include <deque>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>

using namespace downward;
using namespace downward::utils;

namespace probfd::solvers {

namespace {
void print_policy(
    std::ostream& out,
    std::function<void(const State&, std::ostream&)> state_printer,
    std::function<void(const OperatorID&, std::ostream&)> action_printer,
    const Policy<State, OperatorID>& policy,
    MDP<State, OperatorID>& mdp,
    const State& initial_state)
{
    const StateID initial_state_id = mdp.get_state_id(initial_state);

    std::deque<StateID> queue;
    std::set<StateID> visited;
    queue.push_back(initial_state_id);
    visited.insert(initial_state_id);

    do {
        const StateID state_id = queue.front();
        queue.pop_front();

        const State state = mdp.get_state(state_id);

        auto decision = policy.get_decision(state);

        if (!decision) { continue; }

        state_printer(state, out);
        out << " -> ";
        action_printer(decision->action, out);
        out << '\n';

        SuccessorDistribution successor_dist;
        mdp.generate_action_transitions(
            state,
            decision->action,
            successor_dist);

        for (const StateID succ_id :
             successor_dist.non_source_successor_dist.support()) {
            if (visited.insert(succ_id).second) { queue.push_back(succ_id); }
        }
    } while (!queue.empty());
}
} // namespace

MDPSolver::MDPSolver(
    std::shared_ptr<StatisticalMDPAlgorithmFactory> algorithm_factory,
    std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
    std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
    Verbosity verbosity,
    std::string policy_filename,
    bool print_fact_names,
    std::optional<value_t> report_epsilon,
    bool report_enabled)
    : algorithm_factory_(std::move(algorithm_factory))
    , task_state_space_factory_(std::move(task_state_space_factory))
    , heuristic_factory_(std::move(heuristic_factory))
    , log_(get_log_for_verbosity(verbosity))
    , policy_filename(std::move(policy_filename))
    , print_fact_names(print_fact_names)
    , report_epsilon(report_epsilon)
    , report_enabled(report_enabled)
{
}

MDPSolver::~MDPSolver() = default;

class Solver : public SolverInterface {
    std::shared_ptr<ProbabilisticTask> task;
    std::shared_ptr<FDRCostFunction> task_cost_function;

    std::unique_ptr<StatisticalMDPAlgorithm> algorithm;
    std::unique_ptr<TaskStateSpace> state_space;
    const std::shared_ptr<FDREvaluator> heuristic;
    std::string algorithm_name;
    std::string policy_filename;
    bool print_fact_names;

    ProgressReport progress;

public:
    Solver(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        std::unique_ptr<StatisticalMDPAlgorithm> algorithm,
        std::unique_ptr<TaskStateSpace> state_space,
        std::shared_ptr<FDREvaluator> heuristic,
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
        , policy_filename(std::move(policy_filename))
        , print_fact_names(print_fact_names)
        , progress(report_epsilon, std::cout, report_enabled)
    {
    }

    bool solve(double max_time) override
    {
        std::cout << "Running MDP algorithm " << algorithm_name;

        if (max_time != std::numeric_limits<double>::infinity()) {
            std::cout << " with a time limit of " << max_time << " seconds.";
        }

        std::cout << " without a time limit." << std::endl;

        try {
            Timer total_timer;

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

            std::cout << "Finished after " << total_timer() << " [t=" << g_timer
                      << "]" << std::endl;

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
                        out << task->get_fact_name({0, state[0]});
                        for (int i = 1; i != task->get_num_variables(); ++i) {
                            out << ", " << task->get_fact_name({i, state[i]});
                        }
                    } else {
                        out << "{ " << 0 << " -> " << state[0];

                        for (const auto [var, val] :
                             state | as_fact_pair_set | std::views::drop(1)) {
                            out << ", " << var << " -> " << val;
                        }
                        out << " }";
                    }
                };

                auto print_action =
                    [this](const OperatorID& op_id, std::ostream& out) {
                        out << this->task->get_operator_name(op_id.get_index());
                    };

                print_policy(
                    out,
                    print_state,
                    print_action,
                    *policy,
                    mdp,
                    initial_state);
            }

            std::cout << std::endl;
            std::cout << "State space interface statistics:" << std::endl;
            state_space->print_statistics(std::cout);

            std::cout << std::endl;
            std::cout << "Algorithm " << algorithm_name
                      << " statistics:" << std::endl;
            std::cout << "  Actual solver time: " << total_timer << std::endl;
            algorithm->print_statistics(std::cout);

            heuristic->print_statistics();

            return policy != nullptr;
        } catch (TimeoutException&) {
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

    std::unique_ptr<StatisticalMDPAlgorithm> algorithm = run_time_logged(
        std::cout,
        "Constructing algorithm...",
        &StatisticalMDPAlgorithmFactory::create_algorithm,
        *algorithm_factory_,
        task,
        task_cost_function);

    std::unique_ptr<TaskStateSpace> state_space = run_time_logged(
        std::cout,
        "Constructing state space...",
        &TaskStateSpaceFactory::create_state_space,
        *task_state_space_factory_,
        task,
        task_cost_function);

    std::shared_ptr<FDREvaluator> heuristic = run_time_logged(
        std::cout,
        "Constructing heuristic...",
        &TaskHeuristicFactory::create_heuristic,
        *heuristic_factory_,
        task,
        task_cost_function);

    return std::make_unique<Solver>(
        task,
        std::move(task_cost_function),
        std::move(algorithm),
        std::move(state_space),
        std::move(heuristic),
        algorithm_factory_->get_algorithm_name(),
        policy_filename,
        print_fact_names,
        report_epsilon,
        report_enabled);
}

} // namespace probfd::solvers
