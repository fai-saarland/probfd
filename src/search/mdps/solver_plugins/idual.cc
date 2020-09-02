#include "../algorithms/idual.h"

#include "../../lp/lp_solver.h"
#include "../../option_parser.h"
#include "../../plugin.h"
#include "mdp_solver.h"

#include <memory>
#include <string>

namespace probabilistic {
namespace solvers {

class IDual : public MDPSolver {
public:
    explicit IDual(const options::Options& opts)
        : MDPSolver(opts)
        , eval_(opts.get<std::shared_ptr<GlobalStateEvaluator>>("eval"))
        , solver_type_(lp::LPSolverType(opts.get_enum("lpsolver")))
    {
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        parser.add_option<std::shared_ptr<GlobalStateEvaluator>>(
            "eval", "", "const");
        lp::add_lp_solver_option_to_parser(parser);
        MDPSolver::add_options_to_parser(parser);
    }

    virtual std::string name() const override { return "idual"; }

    virtual IMDPEngine<State>* create_engine(
        ProgressReport* report,
        algorithms::StateIDMap<State>* state_id_map,
        algorithms::ApplicableActionsGenerator<State, Action>* aops_generator,
        algorithms::TransitionGenerator<State, Action>* transition_generator,
        value_type::value_t dead_end_value,
        value_type::value_t,
        algorithms::StateEvaluationFunction<State>* state_reward,
        algorithms::TransitionRewardFunction<State, Action>* transition_reward)
        override
    {
        return new algorithms::idual::IDual<State, Action>(
            report,
            state_id_map,
            aops_generator,
            transition_generator,
            state_reward,
            transition_reward,
            dead_end_value,
            eval_.get(),
            solver_type_);
    }

private:
    std::shared_ptr<GlobalStateEvaluator> eval_;
    lp::LPSolverType solver_type_;
};

static Plugin<SolverInterface>
    _plugin("idual", options::parse<SolverInterface, IDual>);

} // namespace solvers
} // namespace probabilistic
