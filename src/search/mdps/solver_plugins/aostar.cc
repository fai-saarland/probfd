#include "../../option_parser.h"
#include "../../plugin.h"
#include "../algorithms/ao_star.h"
#include "../policy_chooser.h"
#include "../state_listener.h"
#include "../transition_sampler.h"
#include "mdp_solver.h"

#include <memory>
#include <string>

namespace probabilistic {
namespace solvers {

class AOStar : public MDPSolver {
public:
    explicit AOStar(const options::Options& opts)
        : MDPSolver(opts)
        , dual_(opts.get<bool>("dual"))
        , successor_sampler_(
              opts.get<std::shared_ptr<TransitionSampler>>("successor_sampler"))
        , dead_end_listener_(GlobalStateComponentListener::from_options(opts))
        , level_(get_dead_end_level(opts))
        , eval_(opts.get<std::shared_ptr<GlobalStateEvaluator>>("eval"))
        , policy_(opts.get<std::shared_ptr<PolicyChooser>>("policy"))
    {
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        parser.add_option<std::shared_ptr<GlobalStateEvaluator>>(
            "eval", "", "const");
        parser.add_option<std::shared_ptr<TransitionSampler>>(
            "successor_sampler", "", "uniform_random");
        parser.add_option<std::shared_ptr<PolicyChooser>>(
            "policy", "", "arbitrary_tiebreaker");
        parser.add_option<bool>("dual", "", "false");
        GlobalStateComponentListener::add_options_to_parser(parser);
        add_dead_end_level_option(parser);
        MDPSolver::add_options_to_parser(parser);
    }

    virtual std::string name() const override { return "aostar"; }

    template<typename B2>
    IMDPEngine<State>* create_engine(
        const B2&,
        ProgressReport* report,
        algorithms::StateIDMap<State>* state_id_map,
        algorithms::ApplicableActionsGenerator<State, Action>* aops_generator,
        algorithms::TransitionGenerator<State, Action>* transition_generator,
        value_type::value_t dead_end_value,
        value_type::value_t,
        algorithms::StateEvaluationFunction<State>* state_reward,
        algorithms::TransitionRewardFunction<State, Action>* transition_reward)
    {
        return new algorithms::ao_star::AOStar<State, Action, B2>(
            successor_sampler_.get(),
            GlobalStateComponentListener::create_state_listener(
                dead_end_listener_, aops_generator, transition_generator),
            level_,
            report,
            state_id_map,
            aops_generator,
            transition_generator,
            state_reward,
            transition_reward,
            eval_.get(),
            policy_.get(),
            dead_end_value,
            this->handler_.get());
    }

    virtual IMDPEngine<State>* create_engine(
        ProgressReport* report,
        algorithms::StateIDMap<State>* state_id_map,
        algorithms::ApplicableActionsGenerator<State, Action>* aops_generator,
        algorithms::TransitionGenerator<State, Action>* transition_generator,
        value_type::value_t dead_end_value,
        value_type::value_t asd,
        algorithms::StateEvaluationFunction<State>* state_reward,
        algorithms::TransitionRewardFunction<State, Action>* transition_reward)
        override
    {
        if (dual_) {
            return create_engine(
                std::true_type(),
                report,
                state_id_map,
                aops_generator,
                transition_generator,
                dead_end_value,
                asd,
                state_reward,
                transition_reward);
        } else {
            return create_engine(
                std::false_type(),
                report,
                state_id_map,
                aops_generator,
                transition_generator,
                dead_end_value,
                asd,
                state_reward,
                transition_reward);
        }
    }

    virtual bool maintains_dual_bounds() const override { return dual_; }

    virtual void print_additional_statistics(std::ostream& out) const override
    {
        if (dead_end_listener_) {
            dead_end_listener_->get_statistics().print(out);
        }
    }

private:
    const bool dual_;
    std::shared_ptr<TransitionSampler> successor_sampler_;
    std::shared_ptr<GlobalStateComponentListener> dead_end_listener_;
    algorithms::DeadEndIdentificationLevel level_;
    std::shared_ptr<GlobalStateEvaluator> eval_;
    std::shared_ptr<PolicyChooser> policy_;
};

static Plugin<SolverInterface>
    _plugin("aostar", options::parse<SolverInterface, AOStar>);

} // namespace solvers
} // namespace probabilistic

