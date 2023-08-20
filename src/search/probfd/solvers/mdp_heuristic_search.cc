#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/algorithms/policy_picker.h"

#include <sstream>

namespace probfd {
namespace solvers {

using namespace algorithms;

MDPHeuristicSearchBase::MDPHeuristicSearchBase(const plugins::Options& opts)
    : MDPSolver(opts)
    , policy_tiebreaker_(opts.get<std::shared_ptr<FDRPolicyPicker>>("policy"))
    , dual_bounds_(
          opts.contains("dual_bounds") && opts.get<bool>("dual_bounds"))
    , interval_comparison_(
          opts.contains("interval_comparison") &&
          opts.get<bool>("interval_comparison"))
{
}

void MDPHeuristicSearchBase::print_additional_statistics() const
{
    if (policy_tiebreaker_) {
        policy_tiebreaker_->print_statistics(std::cout);
    }
}

void MDPHeuristicSearchBase::add_options_to_feature(plugins::Feature& feature)
{
    MDPSolver::add_options_to_feature(feature);

    feature.add_option<std::shared_ptr<FDRPolicyPicker>>(
        "policy",
        "",
        "arbitrary_policy_tiebreaker(false)");
    feature.add_option<bool>("interval_comparison", "", "false");
    feature.add_option<bool>("dual_bounds", "", "false");
}

std::string MDPHeuristicSearchBase::get_algorithm_name() const
{
    return this->get_heuristic_search_name();
}

static plugins::TypedEnumPlugin<FretMode> _fret_enum_plugin(
    {{"disabled", "FRET is disabled"},
     {"policy", "FRET-pi is used"},
     {"value", "FRET-V is used"}});

} // namespace solvers
} // namespace probfd
