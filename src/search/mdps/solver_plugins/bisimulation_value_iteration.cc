#include "../../option_parser.h"
#include "../../plugin.h"
#include "../../utils/timer.h"
#include "../algorithms/topological_value_iteration.h"
#include "../bisimulation/bisimilar_state_space.h"
#include "../globals.h"
#include "mdp_solver.h"

#include <fstream>
#include <memory>
#include <string>

namespace probabilistic {
namespace solvers {

struct BisimulationTimer : public IPrintable {
    utils::Timer timer;
    unsigned states;
    unsigned extended_states;
    unsigned transitions;
    virtual void print(std::ostream& out) const override
    {
        out << "Bisimulation time: " << timer << std::endl;
        out << "Bisimilar states: " << states << " (" << extended_states << ")"
            << std::endl;
        out << "Transitions in bisimulation: " << transitions << std::endl;
    }
};

class BisimulationValueIteration : public IMDPEngine<State> {
public:
    BisimulationValueIteration(
        value_type::value_t dead_end_value,
        value_type::value_t upper)
        : min_(dead_end_value)
        , max_(upper)
    {
    }

    virtual AnalysisResult solve(const State& s) override
    {
        BisimulationTimer stats;

        std::cout << "Computing bisimulation..." << std::endl;
        bisimulation::BisimilarStateSpace bs(s, g_step_bound, g_step_cost_type);
        algorithms::StateIDMap<bisimulation::QuotientState> state_id_map;
        algorithms::ApplicableActionsGenerator<
            bisimulation::QuotientState,
            bisimulation::QuotientAction>
            aops_gen(&bs);
        algorithms::TransitionGenerator<
            bisimulation::QuotientState,
            bisimulation::QuotientAction>
            tgen(&bs);
        algorithms::StateEvaluationFunction<bisimulation::QuotientState>
            state_reward(&bs, min_, max_);
        algorithms::TransitionRewardFunction<
            bisimulation::QuotientState,
            bisimulation::QuotientAction>
            transition_reward;
        stats.timer.stop();
        stats.states = bs.num_bisimilar_states();
        stats.transitions = bs.num_transitions();
        std::cout << "Bisimulation built after " << stats.timer << std::endl;
        std::cout << "Bisimilar state space contains "
                  << bs.num_bisimilar_states() << " states and "
                  << bs.num_transitions() << " transitions." << std::endl;
        // std::ofstream out; out.open("bisim.dot");
        // bs.dump(out);
        // out.close();

        std::cout << "Running value iteration..." << std::endl;
        algorithms::topological_vi::TopologicalValueIteration<
            bisimulation::QuotientState,
            bisimulation::QuotientAction>
            vi(&state_id_map,
               &aops_gen,
               &tgen,
               &state_reward,
               &transition_reward,
               min_,
               max_,
               nullptr);
        AnalysisResult vi_res = vi.solve(bs.get_initial_state());
        stats.extended_states = bs.num_extended_states();
        AnalysisResult res = AnalysisResult(
            vi_res.result,
            new IPrintables(
                { new BisimulationTimer(stats), vi_res.statistics }));
        vi_res.statistics = nullptr;
        return res;
    }

private:
    const value_type::value_t min_;
    const value_type::value_t max_;
};

class BVISolver : public MDPSolver {
public:
    explicit BVISolver(const options::Options& opts)
        : MDPSolver(opts)
    {
    }

    virtual std::string name() const override
    {
        return "bisimiulation_value_iteration";
    }

    virtual IMDPEngine<State>* create_engine(
        ProgressReport*,
        algorithms::StateIDMap<State>*,
        algorithms::ApplicableActionsGenerator<State, Action>*,
        algorithms::TransitionGenerator<State, Action>*,
        value_type::value_t x,
        value_type::value_t y,
        algorithms::StateEvaluationFunction<State>*,
        algorithms::TransitionRewardFunction<State, Action>*) override
    {
        return new BisimulationValueIteration(x, y);
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        MDPSolver::add_options_to_parser(parser);
    }
};

static Plugin<SolverInterface>
    _plugin("bisim_vi", options::parse<SolverInterface, BVISolver>);

} // namespace solvers
} // namespace probabilistic
