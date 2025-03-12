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
        std::shared_ptr<PolicyPicker> policy,
        utils::Verbosity verbosity,
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        std::optional<value_t> report_epsilon,
        bool report_enabled)
    : MDPSolver(
          verbosity,
          std::move(task_state_space_factory),
          std::move(heuristic_factory),
          policy_filename,
          print_fact_names,
          report_epsilon,
          report_enabled)
    , convergence_epsilon_(convergence_epsilon)
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
MDPHeuristicSearch<true, Fret, State, OperatorID>::get_algorithm_name() const
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
    std::shared_ptr<PolicyPicker> policy,
    utils::Verbosity verbosity,
    std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
    std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
    std::string policy_filename,
    bool print_fact_names,
    std::optional<value_t> report_epsilon,
    bool report_enabled)
    : MDPHeuristicSearchBase<false, false, State, Action>(
          convergence_epsilon,
          dual_bounds,
          std::move(policy),
          verbosity,
          std::move(task_state_space_factory),
          std::move(heuristic_factory),
          std::move(policy_filename),
          print_fact_names,
          report_epsilon,
          report_enabled)
{
}

template <typename State, typename Action>
MDPHeuristicSearch<false, true, State, Action>::MDPHeuristicSearch(
    bool fret_on_policy,
    value_t convergence_epsilon,
    bool dual_bounds,
    std::shared_ptr<PolicyPicker> policy,
    utils::Verbosity verbosity,
    std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
    std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
    std::string policy_filename,
    bool print_fact_names,
    std::optional<value_t> report_epsilon,
    bool report_enabled)
    : MDPHeuristicSearchBase<false, true, State, Action>(
          convergence_epsilon,
          dual_bounds,
          std::move(policy),
          verbosity,
          std::move(task_state_space_factory),
          std::move(heuristic_factory),
          std::move(policy_filename),
          print_fact_names,
          report_epsilon,
          report_enabled)
    , fret_on_policy_(fret_on_policy)
{
}

template <bool Fret>
template <typename... Args>
MDPHeuristicSearch<true, Fret, State, OperatorID>::MDPHeuristicSearch(
    Args&&... args)
    : MDPHeuristicSearch<false, Fret, QState, QAction>(
          std::forward<Args>(args)...)
{
}

} // namespace probfd::solvers
