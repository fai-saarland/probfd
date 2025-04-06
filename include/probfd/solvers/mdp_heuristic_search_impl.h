#include "probfd/cli/naming_conventions.h"

#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/quotients/quotient_system.h"

#include <iostream>
#include <sstream>

using namespace probfd::algorithms;

using namespace probfd::cli;

namespace probfd::solvers {

template <bool Bisimulation, bool Fret, typename State, typename Action>
MDPHeuristicSearchBase<Bisimulation, Fret, State, Action>::
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
MDPHeuristicSearch<false, false, State, Action>::get_algorithm_name() const
{
    return this->get_heuristic_search_name();
}

template <typename State, typename Action>
std::string
MDPHeuristicSearch<false, true, State, Action>::get_algorithm_name() const
{
    std::ostringstream out;
    out << "fret" << (fret_on_policy_ ? "_pi" : "_v") << "("
        << this->get_heuristic_search_name() << ")";
    return out.str();
}

template <bool Fret>
std::string
MDPHeuristicSearch<true, Fret, downward::State, downward::OperatorID>::
    get_algorithm_name() const
{
    if constexpr (Fret) {
        std::ostringstream out;
        out << "fret" << (this->fret_on_policy_ ? "_pi" : "_v") << "("
            << this->get_heuristic_search_name() << "(bisimulation)" << ")";
        return out.str();
    } else {
        return this->get_heuristic_search_name() + "(bisimulation)";
    }
}

template <typename State, typename Action>
MDPHeuristicSearch<false, false, State, Action>::MDPHeuristicSearch(
    value_t convergence_epsilon,
    bool dual_bounds,
    std::shared_ptr<PolicyPicker> policy)
    : MDPHeuristicSearchBase<false, false, State, Action>(
          convergence_epsilon,
          dual_bounds,
          std::move(policy))
{
}

template <typename State, typename Action>
MDPHeuristicSearch<false, true, State, Action>::MDPHeuristicSearch(
    bool fret_on_policy,
    value_t convergence_epsilon,
    bool dual_bounds,
    std::shared_ptr<PolicyPicker> policy)
    : MDPHeuristicSearchBase<false, true, State, Action>(
          convergence_epsilon,
          dual_bounds,
          std::move(policy))
    , fret_on_policy_(fret_on_policy)
{
}

} // namespace probfd::solvers
