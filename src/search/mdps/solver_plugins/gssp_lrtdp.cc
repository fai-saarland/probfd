#include "../algorithms/gssp_lrtdp.h"

#include "../../option_parser.h"
#include "../../plugin.h"
#include "../policy_chooser.h"
#include "../state_listener.h"
#include "../transition_sampler.h"
#include "mdp_solver.h"

#include <memory>
#include <string>

namespace probabilistic {
namespace solvers {

class GSSPLRTDP : public MDPSolver {
public:
    explicit GSSPLRTDP(const options::Options& opts)
        : MDPSolver(opts)
        , dual_(opts.get<bool>("dual"))
        , interval_comparison_(opts.get<bool>("interval_comparison"))
        , stop_consistent_(opts.get<bool>("stop_consistent"))
        , successor_sampler_(
              opts.get<std::shared_ptr<TransitionSampler>>("successor_sampler"))
        , dead_end_listener_(GlobalStateComponentListener::from_options(opts))
        , backtrack_eval_(
              opts.contains("backtrack_eval")
                  ? opts.get<std::shared_ptr<GlobalStateEvaluator>>(
                      "backtrack_eval")
                  : nullptr)
        , level_(get_dead_end_level(opts))
        , eval_(opts.get<std::shared_ptr<GlobalStateEvaluator>>("eval"))
        , policy_(opts.get<std::shared_ptr<PolicyChooser>>("policy"))
    {
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        parser.add_option<std::shared_ptr<GlobalStateEvaluator>>(
            "eval", "", "const");
        parser.add_option<std::shared_ptr<GlobalStateEvaluator>>(
            "backtrack_eval", "", options::OptionParser::NONE);
        parser.add_option<bool>("stop_consistent", "", "false");
        parser.add_option<std::shared_ptr<TransitionSampler>>(
            "successor_sampler", "", "uniform_random");
        parser.add_option<std::shared_ptr<PolicyChooser>>(
            "policy", "", "arbitrary_tiebreaker");
        parser.add_option<bool>("dual", "", "false");
        parser.add_option<bool>("interval_comparison", "", "false");
        GlobalStateComponentListener::add_options_to_parser(parser);
        add_dead_end_level_option(parser);
        MDPSolver::add_options_to_parser(parser);
    }

    virtual std::string name() const override { return "gssp_lrtdp"; }

    virtual IMDPEngine<State>* create_engine(
        ProgressReport* report,
        algorithms::StateIDMap<State>* state_id_map,
        algorithms::ApplicableActionsGenerator<State, Action>* aops_generator,
        algorithms::TransitionGenerator<State, Action>* transition_generator,
        value_type::value_t dead_end_value,
        value_type::value_t val,
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
                val,
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
                val,
                state_reward,
                transition_reward);
        }
    }

    template<typename B>
    IMDPEngine<State>* create_engine(
        const B&,
        ProgressReport* report,
        algorithms::StateIDMap<State>* state_id_map,
        algorithms::ApplicableActionsGenerator<State, Action>* aops_generator,
        algorithms::TransitionGenerator<State, Action>* transition_generator,
        value_type::value_t dead_end_value,
        value_type::value_t,
        algorithms::StateEvaluationFunction<State>* state_reward,
        algorithms::TransitionRewardFunction<State, Action>* transition_reward)
    {
        return new algorithms::gssp_lrtdp::LRTDP<State, Action, B>(
            stop_consistent_,
            successor_sampler_.get(),
            GlobalStateComponentListener::create_state_listener(
                dead_end_listener_, aops_generator, transition_generator),
            backtrack_eval_.get(),
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
            this->handler_.get(),
            interval_comparison_);
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
    const bool interval_comparison_;
    const bool stop_consistent_;
    std::shared_ptr<TransitionSampler> successor_sampler_;
    std::shared_ptr<GlobalStateComponentListener> dead_end_listener_;
    std::shared_ptr<GlobalStateEvaluator> backtrack_eval_;
    algorithms::DeadEndIdentificationLevel level_;
    std::shared_ptr<GlobalStateEvaluator> eval_;
    std::shared_ptr<PolicyChooser> policy_;
};

static Plugin<SolverInterface>
    _plugin("gssp_lrtdp", options::parse<SolverInterface, GSSPLRTDP>);

} // namespace solvers
} // namespace probabilistic

