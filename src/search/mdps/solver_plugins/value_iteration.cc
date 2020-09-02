#include "../../option_parser.h"
#include "../../plugin.h"
#include "../algorithms/acyclic_value_iteration.h"
#include "../algorithms/topological_value_iteration.h"
#include "mdp_solver.h"

#include <memory>
#include <string>

namespace probabilistic {
namespace solvers {

class ValueIteration : public MDPSolver {
public:
    explicit ValueIteration(const options::Options& opts)
        : MDPSolver(opts)
        , topological_(opts.get<bool>("topological"))
        , prune_(
              opts.contains("prune")
                  ? opts.get<std::shared_ptr<GlobalStateEvaluator>>("prune")
                  : nullptr)
    {
    }

    virtual std::string name() const override { return "value_iteration"; }

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
        if (topological_) {
            return new algorithms::topological_vi::
                TopologicalValueIteration<State, Action>(
                    state_id_map,
                    aops_generator,
                    transition_generator,
                    state_reward,
                    transition_reward,
                    dead_end_value,
                    upper,
                    prune_.get());
        } else {
            return new algorithms::acyclic_vi::
                AcyclicValueIteration<State, Action>(
                    state_id_map,
                    aops_generator,
                    transition_generator,
                    state_reward,
                    transition_reward,
                    dead_end_value,
                    prune_.get());
        }
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        parser.add_option<std::shared_ptr<GlobalStateEvaluator>>(
            "prune", "", options::OptionParser::NONE);
        parser.add_option<bool>("topological", "", "true");
        MDPSolver::add_options_to_parser(parser);
    }

private:
    const bool topological_;
    std::shared_ptr<GlobalStateEvaluator> prune_;
};

static Plugin<SolverInterface>
    _plugin("vi", options::parse<SolverInterface, ValueIteration>);

} // namespace solvers
} // namespace probabilistic
