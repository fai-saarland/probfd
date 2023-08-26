#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/algorithms/policy_picker.h"

#include "probfd/plugins/naming_conventions.h"

#include <sstream>

namespace probfd {
namespace solvers {

using namespace algorithms;

namespace {
template <
    template <typename, typename>
    typename S,
    bool Bisimulation,
    bool Fret>
using Wrap = std::conditional_t<
    Bisimulation,
    std::conditional_t<
        Fret,
        S<quotients::QuotientState<
              bisimulation::QuotientState,
              bisimulation::QuotientAction>,
          quotients::QuotientAction<bisimulation::QuotientAction>>,
        S<bisimulation::QuotientState, bisimulation::QuotientAction>>,
    std::conditional_t<
        Fret,
        S<quotients::QuotientState<State, OperatorID>,
          quotients::QuotientAction<OperatorID>>,
        S<State, OperatorID>>>;
}

MDPHeuristicSearchBase::MDPHeuristicSearchBase(const plugins::Options& opts)
    : MDPSolver(opts)
    , dual_bounds_(
          opts.contains("dual_bounds") && opts.get<bool>("dual_bounds"))
    , interval_comparison_(
          opts.contains("interval_comparison") &&
          opts.get<bool>("interval_comparison"))
{
}

MDPHeuristicSearch<false, false>::MDPHeuristicSearch(
    const plugins::Options& opts)
    : MDPHeuristicSearchBase(opts)
    , tiebreaker_(opts.get<std::shared_ptr<FDRPolicyPicker>>("policy"))
{
}

MDPHeuristicSearch<false, true>::MDPHeuristicSearch(
    const plugins::Options& opts)
    : MDPHeuristicSearchBase(opts)
    , tiebreaker_(
          opts.get<std::shared_ptr<PolicyPicker<QState, QAction>>>("policy"))
    , fret_on_policy_(opts.get<bool>("fret_on_policy", false))
{
}

MDPHeuristicSearch<true, false>::MDPHeuristicSearch(
    const plugins::Options& opts)
    : MDPHeuristicSearchBase(opts)
    , tiebreaker_(
          opts.get<std::shared_ptr<PolicyPicker<QState, QAction>>>("policy"))
{
}

MDPHeuristicSearch<true, true>::MDPHeuristicSearch(const plugins::Options& opts)
    : MDPHeuristicSearchBase(opts)
    , tiebreaker_(
          opts.get<std::shared_ptr<PolicyPicker<QQState, QQAction>>>("policy"))
    , fret_on_policy_(opts.get<bool>("fret_on_policy"))
{
}

void MDPHeuristicSearch<false, false>::print_additional_statistics() const
{
    tiebreaker_->print_statistics(std::cout);
}

void MDPHeuristicSearch<false, true>::print_additional_statistics() const
{
    tiebreaker_->print_statistics(std::cout);
}

void MDPHeuristicSearch<true, false>::print_additional_statistics() const
{
    tiebreaker_->print_statistics(std::cout);
}

void MDPHeuristicSearch<true, true>::print_additional_statistics() const
{
    tiebreaker_->print_statistics(std::cout);
}

void MDPHeuristicSearchBase::add_options_to_feature(plugins::Feature& feature)
{
    MDPSolver::add_options_to_feature(feature);

    feature.add_option<bool>("interval_comparison", "", "false");
    feature.add_option<bool>("dual_bounds", "", "false");
}

void MDPHeuristicSearch<false, false>::add_options_to_feature(
    plugins::Feature& feature)
{
    MDPHeuristicSearchBase::add_options_to_feature(feature);

    feature.add_option<std::shared_ptr<Wrap<PolicyPicker, false, false>>>(
        "policy",
        "",
        add_mdp_type_to_option<false, false>("arbitrary_policy_tiebreaker()"));
}

void MDPHeuristicSearch<false, true>::add_options_to_feature(
    plugins::Feature& feature)
{
    MDPHeuristicSearchBase::add_options_to_feature(feature);

    feature.add_option<std::shared_ptr<Wrap<PolicyPicker, false, true>>>(
        "policy",
        "",
        add_mdp_type_to_option<false, true>("arbitrary_policy_tiebreaker()"));
}

void MDPHeuristicSearch<true, false>::add_options_to_feature(
    plugins::Feature& feature)
{
    MDPHeuristicSearchBase::add_options_to_feature(feature);

    feature.add_option<std::shared_ptr<Wrap<PolicyPicker, true, false>>>(
        "policy",
        "",
        add_mdp_type_to_option<true, false>("arbitrary_policy_tiebreaker()"));
}

void MDPHeuristicSearch<true, true>::add_options_to_feature(
    plugins::Feature& feature)
{
    MDPHeuristicSearchBase::add_options_to_feature(feature);

    feature.add_option<std::shared_ptr<Wrap<PolicyPicker, true, true>>>(
        "policy",
        "",
        add_mdp_type_to_option<true, true>("arbitrary_policy_tiebreaker()"));
}

std::string MDPHeuristicSearchBase::get_algorithm_name() const
{
    return this->get_heuristic_search_name();
}

} // namespace solvers
} // namespace probfd
