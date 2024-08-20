#include "probfd/solver_interface.h"

#include "probfd/bisimulation/bisimilar_state_space.h"

#include "probfd/algorithms/interval_iteration.h"
#include "probfd/algorithms/topological_value_iteration.h"

#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/tasks/root_task.h"

#include "probfd/progress_report.h"

#include "probfd/tasks/determinization_task.h"

#include "downward/merge_and_shrink/factored_transition_system.h"
#include "downward/merge_and_shrink/merge_and_shrink_representation.h"
#include "downward/merge_and_shrink/transition_system.h"

#include "downward/utils/timer.h"

#include "downward/plugins/plugin.h"

#include <iostream>
#include <memory>
#include <string>

using namespace plugins;
using namespace probfd;
using namespace probfd::bisimulation;

namespace {

merge_and_shrink::Factor
compute_bisimulation_on_determinization(const TaskProxy& det_task_proxy)
{
    utils::Timer timer;

    std::cout << "Computing all-outcomes determinization bisimulation..."
              << std::endl;

    auto factor =
        bisimulation::compute_bisimulation_on_determinization(det_task_proxy);

    std::cout << "AOD-bisimulation was constructed in " << timer << std::endl;

    return factor;
}

static void print_bisimulation_stats(
    std::ostream& out,
    double time,
    unsigned states,
    unsigned transitions)
{
    out << "  Bisimulation time: " << time << std::endl;
    out << "  Bisimilar states: " << states << std::endl;
    out << "  Transitions in bisimulation: " << transitions << std::endl;
}

class BisimulationIteration : public SolverInterface {
    using QState = bisimulation::QuotientState;
    using QAction = bisimulation::QuotientAction;

    const bool interval_iteration_;

public:
    explicit BisimulationIteration(bool interval)
        : interval_iteration_(interval)
    {
    }

    std::string get_algorithm_name() const
    {
        return (
            interval_iteration_ ? "bisimulation interval iteration"
                                : "bisimulation value iteration");
    }

    bool found_solution() const override { return true; }

    void solve() override
    {
        using namespace algorithms::interval_iteration;
        using namespace algorithms::topological_vi;

        const std::shared_ptr<ProbabilisticTask>& task = tasks::g_root_task;
        ProbabilisticTaskProxy task_proxy(*task);

        utils::Timer total_timer;

        std::cout << "Building bisimulation..." << std::endl;

        std::shared_ptr determinization =
            std::make_shared<tasks::DeterminizationTask>(task);

        TaskProxy det_task_proxy(*determinization);

        auto [transition_system, state_mapping, distances] =
            compute_bisimulation_on_determinization(det_task_proxy);

        if (!transition_system->is_solvable(*distances)) {
            std::cout << "Initial state recognized as unsolvable!" << std::endl;
            print_analysis_result(Interval(1_vt, 1_vt));
            std::cout << std::endl;
            return;
        }

        State initial = task_proxy.get_initial_state();
        initial.unpack();
        const auto initial_state =
            QuotientState(state_mapping->get_value(initial));

        utils::Timer timer;

        bisimulation::BisimilarStateSpace state_space(
            task,
            det_task_proxy,
            *transition_system,
            1_vt);

        double time = timer();
        unsigned states = state_space.num_bisimilar_states();
        unsigned transitions = state_space.num_transitions();

        std::cout << "Bisimulation built after " << time << std::endl;
        std::cout << "Bisimilar state space contains " << states
                  << " states and " << transitions << " transitions.\n"
                  << std::endl;

        std::cout << "Running " << get_algorithm_name()
                  << " on the bisimulation..." << std::endl;

        utils::Timer vi_timer;

        std::unique_ptr<MDPAlgorithm<QState, QAction>> solver;

        if (interval_iteration_) {
            solver = std::make_unique<IntervalIteration<QState, QAction>>(
                false,
                false);
        } else {
            solver =
                std::make_unique<TopologicalValueIteration<QState, QAction>>(
                    false);
        }

        heuristics::BlindEvaluator<QState> blind;

        ProgressReport progress;

        const Interval val =
            solver->solve(state_space, blind, initial_state, progress);

        std::cout << "analysis done! [t=" << total_timer << "]" << std::endl;
        std::cout << std::endl;

        print_analysis_result(val);

        std::cout << std::endl;
        std::cout << "Bisimulation:" << std::endl;
        print_bisimulation_stats(std::cout, time, states, transitions);

        std::cout << std::endl;
        std::cout << "Algorithm " << get_algorithm_name()
                  << " statistics:" << std::endl;
        std::cout << "  Actual solver time: " << vi_timer << std::endl;
        solver->print_statistics(std::cout);
    }
};

class BisimulationVISolverFeature
    : public TypedFeature<SolverInterface, BisimulationIteration> {
public:
    BisimulationVISolverFeature()
        : TypedFeature<SolverInterface, BisimulationIteration>(
              "bisimulation_vi")
    {
        document_title("Bisimulation Value Iteration.");
    }

protected:
    std::shared_ptr<BisimulationIteration>
    create_component(const Options&, const utils::Context&) const override
    {
        return std::make_shared<BisimulationIteration>(false);
    }
};

class BisimulationIISolverFeature
    : public TypedFeature<SolverInterface, BisimulationIteration> {
public:
    BisimulationIISolverFeature()
        : TypedFeature<SolverInterface, BisimulationIteration>(
              "bisimulation_ii")
    {
        document_title("Bisimulation Interval Iteration.");
    }

protected:
    std::shared_ptr<BisimulationIteration>
    create_component(const Options&, const utils::Context&) const override
    {
        return std::make_shared<BisimulationIteration>(true);
    }
};

FeaturePlugin<BisimulationVISolverFeature> _plugin_bvi;
FeaturePlugin<BisimulationIISolverFeature> _plugin_bii;

} // namespace
