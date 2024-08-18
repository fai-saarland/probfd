#include "probfd_plugins/naming_conventions.h"

#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/quotients/quotient_system.h"

#include "downward/plugins/plugin.h"

#include <iostream>

using namespace plugins;

using namespace probfd::algorithms;

using namespace probfd_plugins;

namespace probfd::solvers {

template <bool Bisimulation, bool Fret>
MDPHeuristicSearchBase<Bisimulation, Fret>::MDPHeuristicSearchBase(
    bool dual_bounds,
    std::shared_ptr<PolicyPicker> policy,
    utils::Verbosity verbosity,
    const std::shared_ptr<TaskCostFunctionFactory>& costs,
    std::vector<std::shared_ptr<::Evaluator>> path_dependent_evaluators,
    bool cache,
    const std::shared_ptr<TaskEvaluatorFactory>& eval,
    std::optional<value_t> report_epsilon,
    bool report_enabled,
    double max_time,
    std::string policy_filename,
    bool print_fact_names)
    : MDPSolver(
          verbosity,
          costs,
          std::move(path_dependent_evaluators),
          cache,
          eval,
          report_epsilon,
          report_enabled,
          max_time,
          policy_filename,
          print_fact_names)
    , dual_bounds_(dual_bounds)
    , tiebreaker_(std::move(policy))
{
}

template <bool Bisimulation, bool Fret>
void MDPHeuristicSearchBase<Bisimulation, Fret>::print_additional_statistics()
    const
{
    tiebreaker_->print_statistics(std::cout);
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
    const std::shared_ptr<TaskCostFunctionFactory>& costs,
    std::vector<std::shared_ptr<::Evaluator>> path_dependent_evaluators,
    bool cache,
    const std::shared_ptr<TaskEvaluatorFactory>& eval,
    std::optional<value_t> report_epsilon,
    bool report_enabled,
    double max_time,
    std::string policy_filename,
    bool print_fact_names)
    : MDPHeuristicSearchBase(
          dual_bounds,
          std::move(policy),
          verbosity,
          costs,
          std::move(path_dependent_evaluators),
          cache,
          eval,
          report_epsilon,
          report_enabled,
          max_time,
          std::move(policy_filename),
          print_fact_names)
{
}

MDPHeuristicSearch<false, true>::MDPHeuristicSearch(
    bool fret_on_policy,
    bool dual_bounds,
    std::shared_ptr<PolicyPicker> policy,
    utils::Verbosity verbosity,
    const std::shared_ptr<TaskCostFunctionFactory>& costs,
    std::vector<std::shared_ptr<::Evaluator>> path_dependent_evaluators,
    bool cache,
    const std::shared_ptr<TaskEvaluatorFactory>& eval,
    std::optional<value_t> report_epsilon,
    bool report_enabled,
    double max_time,
    std::string policy_filename,
    bool print_fact_names)
    : MDPHeuristicSearchBase(
          dual_bounds,
          std::move(policy),
          verbosity,
          costs,
          std::move(path_dependent_evaluators),
          cache,
          eval,
          report_epsilon,
          report_enabled,
          max_time,
          std::move(policy_filename),
          print_fact_names)
    , fret_on_policy_(fret_on_policy)
{
}

MDPHeuristicSearch<true, false>::MDPHeuristicSearch(
    bool dual_bounds,
    std::shared_ptr<PolicyPicker> policy,
    utils::Verbosity verbosity,
    const std::shared_ptr<TaskCostFunctionFactory>& costs,
    std::vector<std::shared_ptr<::Evaluator>> path_dependent_evaluators,
    bool cache,
    const std::shared_ptr<TaskEvaluatorFactory>& eval,
    std::optional<value_t> report_epsilon,
    bool report_enabled,
    double max_time,
    std::string policy_filename,
    bool print_fact_names)
    : MDPHeuristicSearchBase(
          dual_bounds,
          std::move(policy),
          verbosity,
          costs,
          std::move(path_dependent_evaluators),
          cache,
          eval,
          report_epsilon,
          report_enabled,
          max_time,
          std::move(policy_filename),
          print_fact_names)
{
}

MDPHeuristicSearch<true, true>::MDPHeuristicSearch(
    bool fret_on_policy,
    bool dual_bounds,
    std::shared_ptr<PolicyPicker> policy,
    utils::Verbosity verbosity,
    const std::shared_ptr<TaskCostFunctionFactory>& costs,
    std::vector<std::shared_ptr<::Evaluator>> path_dependent_evaluators,
    bool cache,
    const std::shared_ptr<TaskEvaluatorFactory>& eval,
    std::optional<value_t> report_epsilon,
    bool report_enabled,
    double max_time,
    std::string policy_filename,
    bool print_fact_names)
    : MDPHeuristicSearchBase(
          dual_bounds,
          std::move(policy),
          verbosity,
          costs,
          std::move(path_dependent_evaluators),
          cache,
          eval,
          report_epsilon,
          report_enabled,
          max_time,
          std::move(policy_filename),
          print_fact_names)
    , fret_on_policy_(fret_on_policy)
{
}

} // namespace probfd::solvers
