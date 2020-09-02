#include "../../option_parser.h"
#include "../../plugin.h"
#include "../algorithms/interval_iteration.h"
#include "mdp_solver.h"

#include <memory>
#include <string>

namespace probabilistic {
namespace solvers {

class IntervalIteration : public MDPSolver {
public:
    explicit IntervalIteration(const options::Options& opts)
        : MDPSolver(opts)
        , prune_(
              opts.contains("prune")
                  ? opts.get<std::shared_ptr<GlobalStateEvaluator>>("prune")
                  : nullptr)
        , one_(opts.contains("one_reward"))
        , one_r_(one_ ? opts.get<double>("one_reward") : 0)
    {
    }

    virtual std::string name() const override { return "interval_iteration"; }

    virtual IMDPEngine<State>* create_engine(
        ProgressReport*,
        algorithms::StateIDMap<State>* state_id_map,
        algorithms::ApplicableActionsGenerator<State, Action>* aops_generator,
        algorithms::TransitionGenerator<State, Action>* transition_generator,
        value_type::value_t dead_end_value,
        value_type::value_t upper,
        algorithms::StateEvaluationFunction<State>* state_reward,
        algorithms::TransitionRewardFunction<State, Action>* transition_reward)
        override
    {
            return new algorithms::interval_iteration::
                IntervalIteration<State, Action>(
                    state_id_map,
                    aops_generator,
                    transition_generator,
                    state_reward,
                    transition_reward,
                    dead_end_value,
                    upper,
                    prune_.get(),
                    one_,
                    one_r_);
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        parser.add_option<std::shared_ptr<GlobalStateEvaluator>>(
            "prune", "", options::OptionParser::NONE);
        parser.add_option<double>(
            "one_reward", "", options::OptionParser::NONE);
        MDPSolver::add_options_to_parser(parser);
    }

private:
    std::shared_ptr<GlobalStateEvaluator> prune_;
    const bool one_;
    const value_type::value_t one_r_;
};

static Plugin<SolverInterface>
    _plugin("ii", options::parse<SolverInterface, IntervalIteration>);

} // namespace solvers
} // namespace probabilistic

