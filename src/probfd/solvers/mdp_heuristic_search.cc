#include "probfd/cli/naming_conventions.h"

#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/quotients/quotient_system.h"

#include <iostream>
#include <sstream>

using namespace probfd::algorithms;

using namespace probfd::cli;

namespace probfd::solvers {

template <bool Bisimulation, bool Fret>
MDPHeuristicSearchBase<Bisimulation, Fret>::MDPHeuristicSearchBase(
    bool dual_bounds,
    std::shared_ptr<PolicyPicker> policy,
    utils::Verbosity verbosity,
    std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
    std::shared_ptr<TaskEvaluatorFactory> heuristic_factory,
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
    , dual_bounds_(dual_bounds)
    , tiebreaker_(std::move(policy))
{
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
        << this->get_heuristic_search_name() << "(bisimulation)" << ")";
    return out.str();
}

MDPHeuristicSearch<false, false>::MDPHeuristicSearch(
    bool dual_bounds,
    std::shared_ptr<PolicyPicker> policy,
    utils::Verbosity verbosity,
    std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
    std::shared_ptr<TaskEvaluatorFactory> heuristic_factory,
    std::string policy_filename,
    bool print_fact_names,
    std::optional<value_t> report_epsilon,
    bool report_enabled)
    : MDPHeuristicSearchBase(
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

MDPHeuristicSearch<false, true>::MDPHeuristicSearch(
    bool fret_on_policy,
    bool dual_bounds,
    std::shared_ptr<PolicyPicker> policy,
    utils::Verbosity verbosity,
    std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
    std::shared_ptr<TaskEvaluatorFactory> heuristic_factory,
    std::string policy_filename,
    bool print_fact_names,
    std::optional<value_t> report_epsilon,
    bool report_enabled)
    : MDPHeuristicSearchBase(
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

MDPHeuristicSearch<true, false>::MDPHeuristicSearch(
    bool dual_bounds,
    std::shared_ptr<PolicyPicker> policy,
    utils::Verbosity verbosity,
    std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
    std::shared_ptr<TaskEvaluatorFactory> heuristic_factory,
    std::string policy_filename,
    bool print_fact_names,
    std::optional<value_t> report_epsilon,
    bool report_enabled)
    : MDPHeuristicSearchBase(
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

MDPHeuristicSearch<true, true>::MDPHeuristicSearch(
    bool fret_on_policy,
    bool dual_bounds,
    std::shared_ptr<PolicyPicker> policy,
    utils::Verbosity verbosity,
    std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
    std::shared_ptr<TaskEvaluatorFactory> heuristic_factory,
    std::string policy_filename,
    bool print_fact_names,
    std::optional<value_t> report_epsilon,
    bool report_enabled)
    : MDPHeuristicSearchBase(
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

} // namespace probfd::solvers
