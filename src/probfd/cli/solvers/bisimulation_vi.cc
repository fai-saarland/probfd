#include "probfd/cli/solvers/bisimulation_vi.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "probfd/solver_interface.h"

#include "probfd/bisimulation/bisimilar_state_space.h"

#include "probfd/algorithms/interval_iteration.h"
#include "probfd/algorithms/topological_value_iteration.h"

#include "probfd/heuristics/constant_heuristic.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/tasks/root_task.h"

#include "probfd/progress_report.h"
#include "probfd/task_cost_function.h"

#include "probfd/tasks/determinization_task.h"

#include "probfd/utils/timed.h"

#include "downward/merge_and_shrink/factored_transition_system.h"
#include "downward/merge_and_shrink/merge_and_shrink_representation.h"
#include "downward/merge_and_shrink/transition_system.h"

#include "downward/utils/timer.h"

#include "downward/initial_state_values.h"

#include <iostream>
#include <memory>
#include <string>

using namespace downward;
using namespace probfd;
using namespace probfd::bisimulation;

using namespace downward::cli::plugins;

namespace {
void print_bisimulation_stats(
    std::ostream& out,
    utils::FSeconds time,
    unsigned states,
    unsigned transitions)
{
    std::println(out, "  Bisimulation time: {}", time);
    std::println(out, "  Bisimilar states: {}", states);
    std::println(out, "  Transitions in bisimulation: {}", transitions);
}

class BisimulationSolver : public SolverInterface {
    using QState = bisimulation::QuotientState;
    using QAction = OperatorID;

    SharedProbabilisticTask task;
    std::unique_ptr<MDPAlgorithm<QState, QAction>> solver;
    std::string algorithm_name;

public:
    BisimulationSolver(
        SharedProbabilisticTask task,
        std::unique_ptr<MDPAlgorithm<QState, QAction>> solver,
        std::string algorithm_name)
        : task(std::move(task))
        , solver(std::move(solver))
        , algorithm_name(std::move(algorithm_name))
    {
    }

    bool solve(downward::utils::FSeconds max_time) override
    {
        auto determinization = tasks::create_determinization_task(task);

        std::print(
            std::cout,
            "Computing all-outcomes determinization bisimulation... ");
        auto [transition_system, state_mapping, distances] = run_log_time(
            std::cout,
            bisimulation::compute_bisimulation_on_determinization,
            to_refs(determinization));

        const auto& init_vals = get_init(task);
        const auto& operators = get_operators(task);
        const auto& goals = get_goal(task);
        const auto& cost_function = get_cost_function(task);
        const auto& term_costs = get_termination_costs(task);

        State initial = init_vals.get_initial_state();
        initial.unpack();

        if (!transition_system->is_solvable(*distances)) {
            std::cout << "Initial state recognized as unsolvable!" << std::endl;
            const auto cost = term_costs.get_non_goal_termination_cost();
            print_analysis_result(Interval(cost));
            std::cout << std::endl;
            return false;
        }

        std::print(
            std::cout,
            "Constructing bisimulation abstract state space... ");
        auto [state_space, state_space_time] =
            run_log_time_r(std::cout, [&](const utils::Timer&) {
                return bisimulation::BisimilarStateSpace(
                    task,
                    *transition_system);
            });

        unsigned states = state_space.num_bisimilar_states();
        unsigned transitions = state_space.num_transitions();

        std::cout << "Bisimilar state space contains " << states
                  << " states and " << transitions << " transitions."
                  << std::endl;

        const auto initial_state =
            static_cast<QuotientState>(state_mapping->get_value(initial));

        std::print(
            std::cout,
            "Running {} on the bisimulation...",
            algorithm_name);
        auto [policy, vi_time] =
            run_log_time_r(std::cout, [&](const utils::Timer& vi_timer) {
                heuristics::BlindHeuristic<QState> blind(
                    operators,
                    cost_function,
                    term_costs);

                ProgressReport progress;
                progress.register_print([&](std::ostream& out) {
                    std::print(
                        out,
                        "memory={}",
                        utils::get_peak_memory_in_kib());
                });

                progress.register_print([&](std::ostream& out) {
                    std::print(out, "t={}", vi_timer());
                });

                return solver->compute_policy(
                    state_space,
                    blind,
                    initial_state,
                    progress,
                    max_time);
            });

        std::cout << std::endl;

        auto d = policy->get_decision(initial_state);

        const Interval val =
            d.has_value()
                ? d->q_value_interval
                : Interval(
                      downward::task_properties::is_goal_state(goals, initial)
                          ? term_costs.get_goal_termination_cost()
                          : term_costs.get_non_goal_termination_cost());

        print_analysis_result(val);

        std::cout << std::endl;
        std::cout << "Bisimulation:" << std::endl;
        print_bisimulation_stats(
            std::cout,
            state_space_time,
            states,
            transitions);

        std::cout << std::endl;
        std::cout << "Algorithm " << algorithm_name
                  << " statistics:" << std::endl;
        std::cout << "  Actual solver time: " << vi_time << std::endl;
        solver->print_statistics(std::cout);

        return true;
    }
};

class BisimulationIterationFactory : public TaskSolverFactory {
    using QState = bisimulation::QuotientState;
    using QAction = OperatorID;

    const bool convergence_epsilon_;
    const bool interval_iteration_;

public:
    BisimulationIterationFactory(bool convergence_epsilon, bool interval)
        : convergence_epsilon_(convergence_epsilon)
        , interval_iteration_(interval)
    {
    }

    std::string get_algorithm_name() const
    {
        return (
            interval_iteration_ ? "bisimulation interval iteration"
                                : "bisimulation value iteration");
    }

    std::unique_ptr<SolverInterface>
    create(const SharedProbabilisticTask& task) override
    {
        using namespace algorithms::interval_iteration;
        using namespace algorithms::topological_vi;

        std::unique_ptr<MDPAlgorithm<QState, QAction>> solver;

        if (interval_iteration_) {
            solver = std::make_unique<IntervalIteration<QState, QAction>>(
                convergence_epsilon_,
                false,
                false);
        } else {
            solver =
                std::make_unique<TopologicalValueIteration<QState, QAction>>(
                    convergence_epsilon_,
                    false);
        }

        return std::make_unique<BisimulationSolver>(
            task,
            std::move(solver),
            get_algorithm_name());
    }
};

class BisimulationVISolverFeature
    : public InternalFunctionDefinition<std::shared_ptr<TaskSolverFactory>(value_t)> {
public:
    BisimulationVISolverFeature()
        : InternalFunctionDefinition("bisimulation_vi", &BisimulationVISolverFeature::func)
    {
        document_title("Bisimulation Value Iteration");

        make_optional_argument_with_default(
            0,
            "convergence_epsilon",
            "10e-4",
            "The tolerance for convergence checks.");
    }

protected:
    static std::shared_ptr<TaskSolverFactory> func(value_t convergence_epsilon)
    {
        return std::make_shared<BisimulationIterationFactory>(
            convergence_epsilon,
            false);
    }
};

class BisimulationIISolverFeature
    : public InternalFunctionDefinition<std::shared_ptr<TaskSolverFactory>(value_t)> {
public:
    BisimulationIISolverFeature()
        : InternalFunctionDefinition("bisimulation_ii", &BisimulationIISolverFeature::func)
    {
        document_title("Bisimulation Interval Iteration");

        make_optional_argument_with_default(
            0,
            "convergence_epsilon",
            "10e-4",
            "The tolerance for convergence checks.");
    }

protected:
    static std::shared_ptr<TaskSolverFactory> func(value_t convergence_epsilon)
    {
        return std::make_shared<BisimulationIterationFactory>(
            convergence_epsilon,
            true);
    }
};
} // namespace

namespace probfd::cli::solvers {

void add_bisimulation_value_iteration_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_function_definition<BisimulationVISolverFeature>();
    n.insert_function_definition<BisimulationIISolverFeature>();
}

} // namespace probfd::cli::solvers
