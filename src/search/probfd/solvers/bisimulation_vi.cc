#include "probfd/cost_model.h"

#include "probfd/solver_interface.h"

#include "probfd/bisimulation/bisimilar_state_space.h"
#include "probfd/bisimulation/engine_interfaces.h"

#include "probfd/engines/interval_iteration.h"
#include "probfd/engines/topological_value_iteration.h"

#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/tasks/root_task.h"

#include "downward/utils/timer.h"

#include "downward/state_registry.h"

#include "downward/plugins/plugin.h"

#include <iomanip>

namespace probfd {
namespace solvers {
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

    std::string get_engine_name() const
    {
        return (
            interval_iteration_ ? "bisimulation interval iteration"
                                : "bisimulation value iteration");
    }

    bool found_solution() const override { return true; }

    void solve() override
    {
        using namespace engines::interval_iteration;
        using namespace engines::topological_vi;

        utils::Timer total_timer;

        std::cout << "Building bisimulation..." << std::endl;

        BisimulationTimer stats;
        bisimulation::BisimilarStateSpace state_space(
            tasks::g_root_task.get(),
            g_cost_model->get_cost_function()->get_non_goal_termination_cost());

        stats.timer.stop();
        stats.states = state_space.num_bisimilar_states();
        stats.transitions = state_space.num_transitions();

        std::cout << "Bisimulation built after " << stats.timer << std::endl;
        std::cout << "Bisimilar state space contains "
                  << state_space.num_bisimilar_states() << " states and "
                  << state_space.num_transitions() << " transitions.\n"
                  << std::endl;

        std::cout << "Running " << get_engine_name()
                  << " on the bisimulation..." << std::endl;

        utils::Timer vi_timer;

        std::unique_ptr<MDPEngine<QState, QAction>> solver;

        if (interval_iteration_) {
            solver.reset(new IntervalIteration<QState, QAction>(false, false));
        } else {
            solver.reset(new TopologicalValueIteration<QState, QAction>(false));
        }

        heuristics::BlindEvaluator<QState> blind;

        const Interval val =
            solver->solve(state_space, blind, state_space.get_initial_state());

        std::cout << "analysis done! [t=" << total_timer << "]" << std::endl;
        std::cout << std::endl;

        print_analysis_result(val);

        std::cout << std::endl;
        std::cout << "Bisimulation:" << std::endl;
        stats.print(std::cout);

        std::cout << std::endl;
        std::cout << "Engine " << get_engine_name()
                  << " statistics:" << std::endl;
        std::cout << "  Actual solver time: " << vi_timer << std::endl;
        solver->print_statistics(std::cout);
    }
};

class BisimulationValueIteration : public BisimulationIteration {
public:
    explicit BisimulationValueIteration(const plugins::Options&)
        : BisimulationIteration(false)
    {
    }
};

class BisimulationIntervalIteration : public BisimulationIteration {
public:
    explicit BisimulationIntervalIteration(const plugins::Options&)
        : BisimulationIteration(true)
    {
    }
};

class BisimulationVISolverFeature
    : public plugins::
          TypedFeature<SolverInterface, BisimulationValueIteration> {
public:
    BisimulationVISolverFeature()
        : plugins::TypedFeature<SolverInterface, BisimulationValueIteration>(
              "bisimulation_vi")
    {
        document_title("Bisimulation Value Iteration.");
    }
};

class BisimulationIISolverFeature
    : public plugins::
          TypedFeature<SolverInterface, BisimulationIntervalIteration> {
public:
    BisimulationIISolverFeature()
        : plugins::TypedFeature<SolverInterface, BisimulationIntervalIteration>(
              "bisimulation_ii")
    {
        document_title("Bisimulation Interval Iteration.");
    }
};

static plugins::FeaturePlugin<BisimulationVISolverFeature> _plugin_bvi;
static plugins::FeaturePlugin<BisimulationIISolverFeature> _plugin_bii;

} // namespace
} // namespace solvers
} // namespace probfd
