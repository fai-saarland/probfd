#include "../algorithms/gssp_dohs.h"

#include "../../option_parser.h"
#include "../../plugin.h"
#include "../open_lists/open_list.h"
#include "../policy_chooser.h"
#include "../state_listener.h"
#include "mdp_solver.h"

#include <memory>
#include <string>

namespace probabilistic {
namespace solvers {

class GSSPDOHS : public MDPSolver {
public:
    explicit GSSPDOHS(const options::Options& opts)
        : MDPSolver(opts)
        , dual_(opts.get<bool>("dual"))
        , interval_comparison_(opts.get<bool>("interval_comparison"))
        , forward_updates_(opts.get<bool>("fwup"))
        , backward_updates_(algorithms::gssp_dohs::BacktrackingUpdateType(
              opts.get_enum("bwup")))
        , cutoff_inconsistent_(opts.get<bool>("cutoff_inconsistent"))
        , terminate_inconsistent_(opts.get<bool>("terminate_inconsistent"))
        , value_iteration_(opts.get<bool>("vi"))
        , expand_tip_states_(opts.get<bool>("expand_tip"))
        , mark_solved_(opts.get<bool>("mark_solved"))
        , force_backward_updates_(opts.get<bool>("force_bwup"))
        , open_list_(opts.get<std::shared_ptr<open_lists::GlobalStateOpenList>>(
              "open_list"))
        , dead_end_listener_(GlobalStateComponentListener::from_options(opts))
        , level_(get_dead_end_level(opts))
        , eval_(opts.get<std::shared_ptr<GlobalStateEvaluator>>("eval"))
        , policy_(opts.get<std::shared_ptr<PolicyChooser>>("policy"))
    {
    }

    static void add_core_options_to_parser(options::OptionParser& parser)
    {
        parser.add_option<std::shared_ptr<GlobalStateEvaluator>>(
            "eval", "", "const");
        parser.add_option<std::shared_ptr<PolicyChooser>>(
            "policy", "", "arbitrary_tiebreaker");
        parser.add_option<bool>("dual", "", "false");
        parser.add_option<bool>("interval_comparison", "", "false");
        parser.add_option<std::shared_ptr<open_lists::GlobalStateOpenList>>(
            "open_list", "", "lifo");
        GlobalStateComponentListener::add_options_to_parser(parser);
        add_dead_end_level_option(parser);
        MDPSolver::add_options_to_parser(parser);
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        GSSPDOHS::add_core_options_to_parser(parser);
        parser.add_option<bool>("fwup", "", "true");
        {
            std::vector<std::string> bwups(
                { "disabled", "single", "convergence" });
            parser.add_enum_option("bwup", bwups, "", "single");
        }
        parser.add_option<bool>("cutoff_inconsistent", "", "true");
        parser.add_option<bool>("terminate_inconsistent", "", "false");
        parser.add_option<bool>("vi", "", "false");
        parser.add_option<bool>("expand_tip", "", "true");
        parser.add_option<bool>("force_bwup", "", "true");
        parser.add_option<bool>("mark_solved", "", "true");
    }

    virtual std::string name() const override { return "gssp_dohs"; }

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
        return new algorithms::gssp_dohs::
            DepthOrientedHeuristicSearch<State, Action, B>(
                forward_updates_,
                expand_tip_states_,
                cutoff_inconsistent_,
                terminate_inconsistent_,
                value_iteration_,
                mark_solved_,
                force_backward_updates_,
                backward_updates_,
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
    std::string name_;

    const bool dual_;
    const bool interval_comparison_;

    const bool forward_updates_;
    const algorithms::gssp_dohs::BacktrackingUpdateType backward_updates_;
    const bool cutoff_inconsistent_;
    const bool terminate_inconsistent_;
    const bool value_iteration_;
    const bool expand_tip_states_;
    const bool mark_solved_;
    const bool force_backward_updates_;

    std::shared_ptr<open_lists::GlobalStateOpenList> open_list_;
    std::shared_ptr<GlobalStateComponentListener> dead_end_listener_;
    algorithms::DeadEndIdentificationLevel level_;
    std::shared_ptr<GlobalStateEvaluator> eval_;
    std::shared_ptr<PolicyChooser> policy_;
};

static Plugin<SolverInterface>
    _plugin("gssp_dohs", options::parse<SolverInterface, GSSPDOHS>);

} // namespace solvers
} // namespace probabilistic

