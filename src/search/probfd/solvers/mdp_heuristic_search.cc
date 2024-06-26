#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/plugins/naming_conventions.h"

#include "downward/plugins/plugin.h"

#include <iostream>

namespace probfd::solvers {

using namespace algorithms;
using namespace plugins;

template <bool Bisimulation, bool Fret>
MDPHeuristicSearchBase<Bisimulation, Fret>::MDPHeuristicSearchBase(
    const Options& opts)
    : MDPSolver(opts)
    , dual_bounds_(opts.get<bool>("dual_bounds"))
    , tiebreaker_(
          opts.get<
              std::shared_ptr<WrappedType2<PolicyPicker, Bisimulation, Fret>>>(
              "policy"))
{
}

template <bool Bisimulation, bool Fret>
void MDPHeuristicSearchBase<Bisimulation, Fret>::print_additional_statistics()
    const
{
    tiebreaker_->print_statistics(std::cout);
}

template <bool Bisimulation, bool Fret>
void MDPHeuristicSearchBase<Bisimulation, Fret>::add_options_to_feature(
    Feature& feature)
{
    MDPSolver::add_options_to_feature(feature);

    feature.add_option<bool>("dual_bounds", "", "false");
    feature.add_option<
        std::shared_ptr<WrappedType2<PolicyPicker, Bisimulation, Fret>>>(
        "policy",
        "",
        add_mdp_type_to_option<Bisimulation, Fret>(
            "arbitrary_policy_tiebreaker()"));
}

std::string MDPHeuristicSearch<false, false>::get_algorithm_name() const
{
    return this->get_heuristic_search_name();
}

std::string MDPHeuristicSearch<false, true>::get_algorithm_name() const
{
    std::ostringstream out;
    out << "fret" << (fret_on_policy_ ? "_pi" : "_v") << "("
        << this->get_heuristic_search_name() << ")";
    return out.str();
}

std::string MDPHeuristicSearch<true, false>::get_algorithm_name() const
{
    return get_heuristic_search_name() + "(bisimulation)";
}

std::string MDPHeuristicSearch<true, true>::get_algorithm_name() const
{
    std::ostringstream out;
    out << "fret" << (fret_on_policy_ ? "_pi" : "_v") << "("
        << this->get_heuristic_search_name() << "(bisimulation)"
        << ")";
    return out.str();
}

MDPHeuristicSearch<false, false>::MDPHeuristicSearch(const Options& opts)
    : MDPHeuristicSearchBase(opts)
{
}

MDPHeuristicSearch<false, true>::MDPHeuristicSearch(const Options& opts)
    : MDPHeuristicSearchBase(opts)
    , fret_on_policy_(opts.get<bool>("fret_on_policy", false))
{
}

MDPHeuristicSearch<true, false>::MDPHeuristicSearch(const Options& opts)
    : MDPHeuristicSearchBase(opts)
{
}

MDPHeuristicSearch<true, true>::MDPHeuristicSearch(const Options& opts)
    : MDPHeuristicSearchBase(opts)
    , fret_on_policy_(opts.get<bool>("fret_on_policy"))
{
}

void MDPHeuristicSearch<false, false>::add_options_to_feature(Feature& feature)
{
    MDPHeuristicSearchBase::add_options_to_feature(feature);
}

void MDPHeuristicSearch<false, true>::add_options_to_feature(Feature& feature)
{
    MDPHeuristicSearchBase::add_options_to_feature(feature);
    feature.add_option<bool>("fret_on_policy", "", "true");
}

void MDPHeuristicSearch<true, false>::add_options_to_feature(Feature& feature)
{
    MDPHeuristicSearchBase::add_options_to_feature(feature);
}

void MDPHeuristicSearch<true, true>::add_options_to_feature(Feature& feature)
{
    MDPHeuristicSearchBase::add_options_to_feature(feature);
    feature.add_option<bool>("fret_on_policy", "", "true");
}

} // namespace probfd::solvers
