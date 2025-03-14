#include "probfd/cli/naming_conventions.h"

#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/quotients/quotient_system.h"

#include <iostream>
#include <sstream>

using namespace probfd::algorithms;

using namespace probfd::cli;

namespace probfd::solvers {

template <typename State, typename Action, bool Fret>
MDPHeuristicSearchBase<State, Action, Fret>::
    MDPHeuristicSearchBase(
        value_t convergence_epsilon,
        bool dual_bounds,
        std::shared_ptr<PolicyPicker> policy)
    : convergence_epsilon_(convergence_epsilon)
    , dual_bounds_(dual_bounds)
    , tiebreaker_(std::move(policy))
{
}

template <typename State, typename Action>
std::string
MDPHeuristicSearch<State, Action, false>::get_algorithm_name() const
{
    return this->get_heuristic_search_name();
}

template <typename State, typename Action>
std::string
MDPHeuristicSearch<State, Action, true>::get_algorithm_name() const
{
    std::ostringstream out;
    out << "fret" << (fret_on_policy_ ? "_pi" : "_v") << "("
        << this->get_heuristic_search_name() << ")";
    return out.str();
}

template <typename State, typename Action>
MDPHeuristicSearch<State, Action, true>::MDPHeuristicSearch(
    bool fret_on_policy,
    value_t convergence_epsilon,
    bool dual_bounds,
    std::shared_ptr<PolicyPicker> policy)
    : MDPHeuristicSearchBase<State, Action, true>(
          convergence_epsilon,
          dual_bounds,
          std::move(policy))
    , fret_on_policy_(fret_on_policy)
{
}

} // namespace probfd::solvers
