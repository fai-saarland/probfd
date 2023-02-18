#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/policy_pickers/task_policy_picker_factory.h"

#include <sstream>

namespace probfd {
namespace solvers {

using namespace engine_interfaces;

MDPHeuristicSearchBase::MDPHeuristicSearchBase(const options::Options& opts)
    : MDPSolver(opts)
    , heuristic_(opts.get<std::shared_ptr<TaskStateEvaluator>>("eval"))
    , policy_tiebreaker_(
          opts.contains("policy")
              ? opts.get<std::shared_ptr<TaskPolicyPickerFactory>>("policy")
                    ->create_policy_tiebreaker(
                        &this->state_id_map_,
                        &this->action_id_map_)
              : nullptr)
    , new_state_handler_(new TaskNewStateHandlerList(
          opts.get_list<std::shared_ptr<TaskNewStateHandler>>("on_new_state")))
    , dual_bounds_(
          opts.contains("dual_bounds") && opts.get<bool>("dual_bounds"))
    , interval_comparison_(
          opts.contains("interval_comparison") &&
          opts.get<bool>("interval_comparison"))
    , stable_policy_(
          opts.contains("stable_policy") && opts.get<bool>("stable_policy"))
{
}

void MDPHeuristicSearchBase::print_additional_statistics() const
{
    heuristic_->print_statistics();

    if (policy_tiebreaker_) {
        policy_tiebreaker_->print_statistics(std::cout);
    }
}

void MDPHeuristicSearchBase::add_options_to_parser(
    options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<TaskStateEvaluator>>("eval", "", "const");
    parser.add_list_option<std::shared_ptr<TaskNewStateHandler>>(
        "on_new_state",
        "",
        "[]");
    parser.add_option<std::shared_ptr<TaskPolicyPickerFactory>>(
        "policy",
        "",
        "arbitrary_policy_tiebreaker_factory");
    parser.add_option<bool>("interval_comparison", "", "false");
    parser.add_option<bool>("stable_policy", "", "false");
    parser.add_option<bool>("dual_bounds", "", "false");
    MDPSolver::add_options_to_parser(parser);
}

std::string MDPHeuristicSearchBase::get_engine_name() const
{
    return this->get_heuristic_search_name();
}

} // namespace solvers
} // namespace probfd
