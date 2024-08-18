#include "probfd/solver_interface.h"

#include "probfd/bisimulation/bisimilar_state_space.h"

#include "probfd/algorithms/interval_iteration.h"
#include "probfd/algorithms/topological_value_iteration.h"

#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/tasks/root_task.h"

#include "probfd/policy.h"
#include "probfd/progress_report.h"

#include "downward/utils/timer.h"

#include "downward/plugins/plugin.h"

#include <iostream>
#include <memory>
#include <string>

using namespace plugins;
using namespace probfd;

namespace {

struct BisimulationTimer {
    utils::Timer timer;
    unsigned states;
    unsigned transitions;
    void print(std::ostream& out) const
    {
        out << "  Bisimulation time: " << timer << std::endl;
        out << "  Bisimilar states: " << states << std::endl;
        out << "  Transitions in bisimulation: " << transitions << std::endl;
    }
};

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

        utils::Timer total_timer;

        std::cout << "Building bisimulation..." << std::endl;

        BisimulationTimer stats;
        bisimulation::BisimilarStateSpace state_space(
            tasks::g_root_task.get(),
            1_vt);

        stats.timer.stop();
        stats.states = state_space.num_bisimilar_states();
        stats.transitions = state_space.num_transitions();

        std::cout << "Bisimulation built after " << stats.timer << std::endl;
        std::cout << "Bisimilar state space contains "
                  << state_space.num_bisimilar_states() << " states and "
                  << state_space.num_transitions() << " transitions.\n"
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

        const Interval val = solver->solve(
            state_space,
            blind,
            state_space.get_initial_state(),
            progress);

        std::cout << "analysis done! [t=" << total_timer << "]" << std::endl;
        std::cout << std::endl;

        print_analysis_result(val);

        std::cout << std::endl;
        std::cout << "Bisimulation:" << std::endl;
        stats.print(std::cout);

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
