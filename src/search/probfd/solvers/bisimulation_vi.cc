#include "probfd/cost_model.h"

#include "probfd/solver_interface.h"

#include "probfd/bisimulation/bisimilar_state_space.h"
#include "probfd/bisimulation/engine_interfaces.h"

#include "probfd/engines/interval_iteration.h"
#include "probfd/engines/topological_value_iteration.h"

#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/tasks/root_task.h"

#include "utils/timer.h"

#include "state_registry.h"

#include "option_parser.h"
#include "plugin.h"

#include <iomanip>

namespace probfd {
namespace solvers {
namespace {

using namespace engine_interfaces;

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

    static void add_options_to_parser(options::OptionParser&) {}

    std::string get_engine_name() const
    {
        return (
            interval_iteration_ ? "bisimulation interval iteration"
                                : "bisimulation value iteration");
    }

    virtual bool found_solution() const override { return true; }

    virtual void solve() override
    {
        utils::Timer total_timer;

        std::cout << "Building bisimulation..." << std::endl;

        BisimulationTimer stats;
        bisimulation::BisimilarStateSpace bs(tasks::g_root_task.get());
        StateSpace<QState, QAction> state_space(&bs);
        bisimulation::DefaultQuotientCostFunction cost(
            &bs,
            g_cost_model->optimal_value_bound(),
            g_cost_model->optimal_value_bound().upper);

        stats.timer.stop();
        stats.states = bs.num_bisimilar_states();
        stats.transitions = bs.num_transitions();

        std::cout << "Bisimulation built after " << stats.timer << std::endl;
        std::cout << "Bisimilar state space contains "
                  << bs.num_bisimilar_states() << " states and "
                  << bs.num_transitions() << " transitions." << std::endl;
        std::cout << std::endl;
        std::cout << "Running " << get_engine_name()
                  << " on the bisimulation..." << std::endl;

        utils::Timer vi_timer;
        std::unique_ptr<engines::MDPEngine<QState, QAction>> solver;
        Interval val;
        if (interval_iteration_) {
            solver.reset(new engines::interval_iteration::IntervalIteration<
                         QState,
                         QAction>(&state_space, &cost, nullptr, false, false));
            val = solver->solve(bs.get_initial_state());
        } else {
            heuristics::ConstantEvaluator<QState> initializer(0_vt);
            solver.reset(new engines::topological_vi::TopologicalValueIteration<
                         QState,
                         QAction>(&state_space, &cost, &initializer, false));
            val = solver->solve(bs.get_initial_state());
        }
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
    explicit BisimulationValueIteration(const options::Options&)
        : BisimulationIteration(false)
    {
    }
    static void add_options_to_parser(options::OptionParser&) {}
};

class BisimulationIntervalIteration : public BisimulationIteration {
public:
    explicit BisimulationIntervalIteration(const options::Options&)
        : BisimulationIteration(true)
    {
    }
    static void add_options_to_parser(options::OptionParser&) {}
};

static Plugin<SolverInterface> _plugin(
    "bisimulation_vi",
    options::parse<SolverInterface, BisimulationValueIteration>);

static Plugin<SolverInterface> _plugin_ii(
    "bisimulation_ii",
    options::parse<SolverInterface, BisimulationIntervalIteration>);

} // namespace
} // namespace solvers
} // namespace probfd
