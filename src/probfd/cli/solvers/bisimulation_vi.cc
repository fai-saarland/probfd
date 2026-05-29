#include "probfd/cli/solvers/bisimulation_vi.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/probabilistic_task_solver.h"

#include "probfd/bisimulation/bisimilar_state_space.h"

#include "probfd/algorithms/interval_iteration.h"
#include "probfd/algorithms/topological_value_iteration.h"

#include "probfd/heuristics/blind_heuristic.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/tasks/root_task.h"

#include "probfd/progress_report.h"
#include "probfd/task_cost_function.h"
#include "probfd/termination_costs.h"

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

using namespace language::plugins;

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

class BisimulationSolver : public ProbabilisticTaskSolver {
    using QState = bisimulation::QuotientState;
    using QAction = OperatorID;

    std::unique_ptr<MDPAlgorithm<QState, QAction>> solver;
    std::string algorithm_name;

public:
    BisimulationSolver(
        std::unique_ptr<MDPAlgorithm<QState, QAction>> solver,
        std::string algorithm_name)
        : solver(std::move(solver))
        , algorithm_name(std::move(algorithm_name))
    {
    }

    bool solve(const SharedProbabilisticTask& task, utils::FSeconds max_time)
        override
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

class BisimulationVISolverFeature
    : public InternalFunctionDefinition<
          std::shared_ptr<ProbabilisticTaskSolver>(value_t)> {
    using QState = bisimulation::QuotientState;
    using QAction = OperatorID;

public:
    BisimulationVISolverFeature()
        : InternalFunctionDefinition(
              "bisimulation_vi",
              &BisimulationVISolverFeature::func)
    {
        document_title("Bisimulation Value Iteration");

        make_optional_argument_with_default(
            0,
            "convergence_epsilon",
            "10e-4",
            "The tolerance for convergence checks.");
    }

protected:
    static std::shared_ptr<ProbabilisticTaskSolver>
    func(value_t convergence_epsilon)
    {
        return std::make_shared<BisimulationSolver>(
            std::make_unique<algorithms::topological_vi::
                                 TopologicalValueIteration<QState, QAction>>(
                convergence_epsilon,
                false),
            "bisimulation value iteration");
    }
};

class BisimulationIISolverFeature
    : public InternalFunctionDefinition<
          std::shared_ptr<ProbabilisticTaskSolver>(value_t)> {
    using QState = bisimulation::QuotientState;
    using QAction = OperatorID;

public:
    BisimulationIISolverFeature()
        : InternalFunctionDefinition(
              "bisimulation_ii",
              &BisimulationIISolverFeature::func)
    {
        document_title("Bisimulation Interval Iteration");

        make_optional_argument_with_default(
            0,
            "convergence_epsilon",
            "10e-4",
            "The tolerance for convergence checks.");
    }

protected:
    static std::shared_ptr<ProbabilisticTaskSolver>
    func(value_t convergence_epsilon)
    {
        return std::make_shared<BisimulationSolver>(
            std::make_unique<algorithms::interval_iteration::
                                 IntervalIteration<QState, QAction>>(
                convergence_epsilon,
                false,
                false),
            "bisimulation interval iteration");
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
