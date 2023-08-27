#include "probfd/solver_interface.h"

#include "probfd/bisimulation/bisimilar_state_space.h"
#include "probfd/bisimulation/evaluators.h"


#include "probfd/algorithms/interval_iteration.h"
#include "probfd/algorithms/topological_value_iteration.h"

#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/tasks/root_task.h"

#include "downward/utils/timer.h"

#include "downward/state_registry.h"

#include "downward/plugins/plugin.h"

#include <iomanip>

namespace probfd {
namespace solvers {
namespace {

using namespace plugins;

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
        std::cout << "Algorithm " << get_algorithm_name()
                  << " statistics:" << std::endl;
        std::cout << "  Actual solver time: " << vi_timer << std::endl;
        solver->print_statistics(std::cout);
    }
};

class BisimulationValueIteration : public BisimulationIteration {
public:
    explicit BisimulationValueIteration(const Options&)
        : BisimulationIteration(false)
    {
    }
};

class BisimulationIntervalIteration : public BisimulationIteration {
public:
    explicit BisimulationIntervalIteration(const Options&)
        : BisimulationIteration(true)
    {
    }
};

class BisimulationVISolverFeature
    : public TypedFeature<SolverInterface, BisimulationValueIteration> {
public:
    BisimulationVISolverFeature()
        : TypedFeature<SolverInterface, BisimulationValueIteration>(
              "bisimulation_vi")
    {
        document_title("Bisimulation Value Iteration.");
    }
};

class BisimulationIISolverFeature
    : public TypedFeature<SolverInterface, BisimulationIntervalIteration> {
public:
    BisimulationIISolverFeature()
        : TypedFeature<SolverInterface, BisimulationIntervalIteration>(
              "bisimulation_ii")
    {
        document_title("Bisimulation Interval Iteration.");
    }
};

static FeaturePlugin<BisimulationVISolverFeature> _plugin_bvi;
static FeaturePlugin<BisimulationIISolverFeature> _plugin_bii;

} // namespace
} // namespace solvers
} // namespace probfd
