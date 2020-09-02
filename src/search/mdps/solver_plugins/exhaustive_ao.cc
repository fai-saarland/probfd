#include "../../option_parser.h"
#include "../../plugin.h"
#include "../algorithms/ao_exhaustive_generic.h"
#include "../open_lists/open_list.h"
#include "../policy_chooser.h"
#include "../state_listener.h"
#include "mdp_solver.h"

#include <memory>
#include <string>

namespace probabilistic {
namespace solvers {

class ExhaustiveAO : public MDPSolver {
public:
    explicit ExhaustiveAO(const options::Options& opts)
        : MDPSolver(opts)
        , dual_(opts.get<bool>("dual"))
        , interval_comparison_(opts.get<bool>("interval_comparison"))
        , open_list_(opts.get<std::shared_ptr<open_lists::GlobalStateOpenList>>(
              "open_list"))
        , dead_end_listener_(GlobalStateComponentListener::from_options(opts))
        , level_(get_dead_end_level(opts))
        , eval_(opts.get<std::shared_ptr<GlobalStateEvaluator>>("eval"))
    {
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        parser.add_option<std::shared_ptr<GlobalStateEvaluator>>(
            "eval", "", "const");
        parser.add_option<std::shared_ptr<open_lists::GlobalStateOpenList>>(
            "open_list", "", "lifo");
        parser.add_option<bool>("dual", "", "false");
        parser.add_option<bool>("interval_comparison", "", "false");
        GlobalStateComponentListener::add_options_to_parser(parser);
        add_dead_end_level_option(parser);
        MDPSolver::add_options_to_parser(parser);
    }

    virtual std::string name() const override { return "exhaustive_ao"; }

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
        return new algorithms::exhaustive_ao::
            GenericExhaustiveAOSearch<State, Action, B>(
                open_list_.get(),
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
                nullptr,
                dead_end_value,
                this->handler_.get(),
                interval_comparison_);
    }

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
    std::shared_ptr<open_lists::GlobalStateOpenList> open_list_;
    std::shared_ptr<GlobalStateComponentListener> dead_end_listener_;
    algorithms::DeadEndIdentificationLevel level_;
    std::shared_ptr<GlobalStateEvaluator> eval_;
};

static Plugin<SolverInterface>
    _plugin("exhaustive_ao", options::parse<SolverInterface, ExhaustiveAO>);

} // namespace solvers
} // namespace probabilistic

