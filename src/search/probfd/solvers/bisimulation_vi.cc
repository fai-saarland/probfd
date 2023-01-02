#include "probfd/analysis_objectives/analysis_objective.h"

#include "probfd/bisimulation/bisimilar_state_space.h"
#include "probfd/bisimulation/engine_interfaces.h"

#include "probfd/engines/interval_iteration.h"
#include "probfd/engines/topological_value_iteration.h"

#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/solvers/solver_interface.h"

#include "probfd/utils/logging.h"

#include "probfd/tasks/root_task.h"

#include "utils/timer.h"

#include "state_registry.h"

#include "option_parser.h"
#include "plugin.h"

#include <iomanip>

namespace probfd {
namespace solvers {

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

public:
    explicit BisimulationIteration(bool interval)
        : interval_iteration_(interval)
    {
    }

    static void add_options_to_parser(options::OptionParser&) {}

    std::string get_engine_name() const
    {
        return (interval_iteration_ ? "interval iteration" : "value iteration");
    }

    virtual bool found_solution() const override { return true; }

    virtual void solve() override
    {
        utils::Timer total_timer;

        logging::out << "Building bisimulation..." << std::endl;

        BisimulationTimer stats;
        bisimulation::BisimilarStateSpace bs(tasks::g_root_task.get());
        StateIDMap<QState> state_id_map;
        ActionIDMap<QAction> action_id_map;
        TransitionGenerator<QAction> tgen(&bs);
        bisimulation::DefaultQuotientRewardFunction reward(
            &bs,
            g_analysis_objective->reward_bound());

        stats.timer.stop();
        stats.states = bs.num_bisimilar_states();
        stats.transitions = bs.num_transitions();

        logging::out << "Bisimulation built after " << stats.timer << std::endl;
        logging::out << "Bisimilar state space contains "
                     << bs.num_bisimilar_states() << " states and "
                     << bs.num_transitions() << " transitions." << std::endl;
        logging::out << std::endl;
        logging::out << "Running " << get_engine_name()
                     << " on the bisimulation..." << std::endl;
        utils::Timer vi_timer;
        engines::MDPEngine<QState, QAction>* solver = nullptr;
        if (interval_iteration_) {
            solver = new engines::interval_iteration::
                IntervalIteration<QState, QAction>(
                    &state_id_map,
                    &action_id_map,
                    &reward,
                    &tgen,
                    nullptr,
                    false,
                    false);
        } else {
            heuristics::ConstantEvaluator<QState> initializer(value_type::zero);
            solver = new engines::topological_vi::
                TopologicalValueIteration<QState, QAction>(
                    &state_id_map,
                    &action_id_map,
                    &reward,
                    &tgen,
                    &initializer,
                    false);
        }
        value_type::value_t val = solver->solve(bs.get_initial_state());
        logging::out << "analysis done! [t=" << total_timer << "]" << std::endl;
        logging::out << std::endl;

        logging::print_analysis_result(val);

        logging::out << std::endl;
        logging::out << "Bisimulation:" << std::endl;
        stats.print(logging::out);

        logging::out << std::endl;
        logging::out << "Engine " << get_engine_name()
                     << " statistics:" << std::endl;
        logging::out << "  Actual solver time: " << vi_timer << std::endl;
        solver->print_statistics(logging::out);

        delete (solver);
    }

    const bool interval_iteration_;
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

} // namespace solvers
} // namespace probfd
