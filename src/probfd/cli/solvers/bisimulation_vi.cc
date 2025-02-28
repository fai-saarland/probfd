#include "downward/cli/plugins/plugin.h"

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

#include <iostream>
#include <memory>
#include <string>

using namespace probfd;
using namespace probfd::bisimulation;

using namespace downward::cli::plugins;

namespace {

void print_bisimulation_stats(
    std::ostream& out,
    double time,
    unsigned states,
    unsigned transitions)
{
    out << "  Bisimulation time: " << time << std::endl;
    out << "  Bisimilar states: " << states << std::endl;
    out << "  Transitions in bisimulation: " << transitions << std::endl;
}

class BisimulationSolver : public SolverInterface {
    using QState = bisimulation::QuotientState;
    using QAction = OperatorID;

    std::shared_ptr<ProbabilisticTask> task;
    std::shared_ptr<FDRCostFunction> task_cost_function;
    std::unique_ptr<MDPAlgorithm<QState, QAction>> solver;
    std::string algorithm_name;

public:
    BisimulationSolver(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        std::unique_ptr<MDPAlgorithm<QState, QAction>> solver,
        std::string algorithm_name)
        : task(std::move(task))
        , task_cost_function(std::move(task_cost_function))
        , solver(std::move(solver))
        , algorithm_name(std::move(algorithm_name))
    {
    }

    bool solve(double max_time) override
    {
        utils::Timer total_timer;

        std::cout << "Building bisimulation..." << std::endl;

        std::shared_ptr determinization =
            std::make_shared<tasks::DeterminizationTask>(task);

        TaskProxy det_task_proxy(*determinization);

        auto [transition_system, state_mapping, distances] = timed(
            std::cout,
            "Computing all-outcomes determinization bisimulation...",
            bisimulation::compute_bisimulation_on_determinization,
            det_task_proxy);

        if (!transition_system->is_solvable(*distances)) {
            std::cout << "Initial state recognized as unsolvable!" << std::endl;
            const auto cost = task_cost_function->get_termination_cost(initial);
            print_analysis_result(Interval(cost));
            std::cout << std::endl;
            return false;
        }

        ProbabilisticTaskProxy task_proxy(*task);
        State initial = task_proxy.get_initial_state();
        initial.unpack();
        const auto initial_state =
            QuotientState(state_mapping->get_value(initial));

        utils::Timer timer;

        bisimulation::BisimilarStateSpace state_space(
            task,
            task_cost_function,
            *transition_system);

        double time = timer();
        unsigned states = state_space.num_bisimilar_states();
        unsigned transitions = state_space.num_transitions();

        std::cout << "Bisimulation built after " << time << std::endl;
        std::cout << "Bisimilar state space contains " << states
                  << " states and " << transitions << " transitions.\n"
                  << std::endl;

        std::cout << "Running " << algorithm_name << " on the bisimulation..."
                  << std::endl;

        utils::Timer vi_timer;

        ProbabilisticOperatorsProxy ops(*task);
        heuristics::BlindEvaluator<QState> blind(ops, *task_cost_function);
        ProgressReport progress;

        const auto policy = solver->compute_policy(
            state_space,
            blind,
            initial_state,
            progress,
            max_time);

        auto d = policy->get_decision(initial_state);

        const Interval val =
            d.has_value()
                ? d->q_value_interval
                : Interval(task_cost_function->get_termination_cost(initial));

        std::cout << "Finished after " << vi_timer() << " [t=" << total_timer
                  << "]" << std::endl;
        std::cout << std::endl;

        print_analysis_result(val);

        std::cout << std::endl;
        std::cout << "Bisimulation:" << std::endl;
        print_bisimulation_stats(std::cout, time, states, transitions);

        std::cout << std::endl;
        std::cout << "Algorithm " << algorithm_name
                  << " statistics:" << std::endl;
        std::cout << "  Actual solver time: " << vi_timer << std::endl;
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

    virtual std::unique_ptr<SolverInterface>
    create(const std::shared_ptr<ProbabilisticTask>& task)
    {
        using namespace algorithms::interval_iteration;
        using namespace algorithms::topological_vi;

        auto task_cost_function = std::make_shared<TaskCostFunction>(task);

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
            std::move(task_cost_function),
            std::move(solver),
            get_algorithm_name());
    }
};

class BisimulationVISolverFeature
    : public TypedFeature<TaskSolverFactory, BisimulationIterationFactory> {
public:
    BisimulationVISolverFeature()
        : TypedFeature<TaskSolverFactory, BisimulationIterationFactory>(
              "bisimulation_vi")
    {
        document_title("Bisimulation Value Iteration");

        add_option<value_t>(
            "convergence_epsilon",
            "The tolerance for convergence checks.",
            "10e-4");
    }

protected:
    std::shared_ptr<BisimulationIterationFactory>
    create_component(const Options& opts, const utils::Context&) const override
    {
        return std::make_shared<BisimulationIterationFactory>(
            opts.get<value_t>("convergence_epsilon"),
            false);
    }
};

class BisimulationIISolverFeature
    : public TypedFeature<TaskSolverFactory, BisimulationIterationFactory> {
public:
    BisimulationIISolverFeature()
        : TypedFeature<TaskSolverFactory, BisimulationIterationFactory>(
              "bisimulation_ii")
    {
        document_title("Bisimulation Interval Iteration");

        add_option<value_t>(
            "convergence_epsilon",
            "The tolerance for convergence checks.",
            "10e-4");
    }

protected:
    std::shared_ptr<BisimulationIterationFactory>
    create_component(const Options& opts, const utils::Context&) const override
    {
        return std::make_shared<BisimulationIterationFactory>(
            opts.get<value_t>("convergence_epsilon"),
            true);
    }
};

FeaturePlugin<BisimulationVISolverFeature> _plugin_bvi;
FeaturePlugin<BisimulationIISolverFeature> _plugin_bii;

} // namespace
