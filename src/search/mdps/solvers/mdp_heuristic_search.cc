#include "mdp_heuristic_search.h"

#include "../logging.h"

#include <sstream>

namespace probabilistic {

MDPHeuristicSearchBase::MDPHeuristicSearchBase(const options::Options& opts)
    : MDPSolver(opts)
    , connector_()
    , policy_tiebreaker_(
          opts.contains("policy")
              ? opts.get<std::shared_ptr<ProbabilisticOperatorPolicyPicker>>(
                  "policy")
              : nullptr)
    , new_state_handler_(new NewGlobalStateHandlerList(
          opts.get_list<std::shared_ptr<NewGlobalStateHandler>>(
              "on_new_state")))
    , heuristic_(opts.get<std::shared_ptr<GlobalStateEvaluator>>("eval"))
    , dead_end_listener_(
          opts.contains("on_dead_end")
              ? new DeadEndListener<GlobalState, const ProbabilisticOperator*>(
                  opts.get<
                      std::shared_ptr<state_component::StateComponentListener>>(
                      "on_dead_end"),
                  this->get_state_id_map(),
                  this->get_transition_generator())
              : nullptr)
    , dead_end_eval_(
          opts.contains("dead_end_eval")
              ? opts.get<std::shared_ptr<GlobalStateEvaluator>>("dead_end_eval")
              : nullptr)
    , dead_end_ident_level_(DeadEndIdentificationLevel(
          opts.contains("dead_end_identification")
              ? opts.get_enum("dead_end_identification")
              : 0))
    , dual_bounds_(
          opts.contains("dual_bounds") && opts.get<bool>("dual_bounds"))
    , interval_comparison_(
          opts.contains("interval_comparison")
          && opts.get<bool>("interval_comparison"))
    , stable_policy_(
          opts.contains("stable_policy") && opts.get<bool>("stable_policy"))
{
    if (policy_tiebreaker_) {
        policy_tiebreaker_->initialize(
            &connector_, this->get_state_id_map(), this->get_action_id_map());
    }
}

void
MDPHeuristicSearchBase::initialize_interfaceable(
    HeuristicSearchInterfaceable* interfaceable)
{
    if (interfaceable) {
        interfaceable->initialize(
            &connector_, this->get_state_id_map(), this->get_action_id_map());
    }
}

void
MDPHeuristicSearchBase::initialize_interfaceable(
    std::shared_ptr<HeuristicSearchInterfaceable> interfaceable)
{
    if (interfaceable) {
        interfaceable->initialize(
            &connector_, this->get_state_id_map(), this->get_action_id_map());
    }
}

void
MDPHeuristicSearchBase::print_additional_statistics() const
{
    if (policy_tiebreaker_) {
        policy_tiebreaker_->print_statistics(logging::out);
    }
    heuristic_->print_statistics();
    if (dead_end_listener_ != nullptr) {
        dead_end_listener_->print_statistics(logging::out);
    }
}

void
MDPHeuristicSearchBase::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<GlobalStateEvaluator>>(
        "eval", "", "const");
    parser.add_list_option<std::shared_ptr<NewGlobalStateHandler>>(
        "on_new_state", "", "[]");
    parser.add_option<std::shared_ptr<ProbabilisticOperatorPolicyPicker>>(
        "policy", "", "arbitrary_policy_tiebreaker");
    parser.add_option<bool>("interval_comparison", "", "false");
    parser.add_option<bool>("stable_policy", "", "false");
    parser.add_option<bool>("dual_bounds", "", "false");
    parser.add_option<std::shared_ptr<GlobalStateEvaluator>>(
        "dead_end_eval", "", options::OptionParser::NONE);
    parser.add_option<std::shared_ptr<state_component::StateComponentListener>>(
        "on_dead_end", "", options::OptionParser::NONE);
    std::vector<std::string> levels(
        { "disabled", "policy", "visited", "complete" });
    parser.add_enum_option("dead_end_identification", levels, "", "disabled");
    MDPSolver::add_options_to_parser(parser);
}

std::string
MDPHeuristicSearchBase::get_engine_name() const
{
    return this->get_heuristic_search_name();
}

} // namespace probabilistic
