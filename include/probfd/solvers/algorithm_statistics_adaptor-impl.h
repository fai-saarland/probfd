#ifndef PROBFD_SOLVERS_ALGORITHM_STATISTICS_ADAPTOR_GUARD
#error "This file should only be included from algorithm_statistics_adaptor.h"
#endif

#include "probfd/solvers/statistical_mdp_algorithm.h"

#include "probfd/mdp_algorithm.h"
#include "probfd/progress_report.h"

#include "downward/operator_id.h"
#include "downward/task_proxy.h"

#include <iostream>

namespace probfd::solvers {

template <typename State, typename Action>
AlgorithmAdaptor<State, Action>::AlgorithmAdaptor(
    std::unique_ptr<MDPAlgorithm<State, Action>> algorithm)
    : algorithm(std::move(algorithm))
{
}

template <typename State, typename Action>
AlgorithmAdaptor<State, Action>::~AlgorithmAdaptor() = default;

template <typename State, typename Action>
auto AlgorithmAdaptor<State, Action>::compute_policy(
    MDPType& mdp,
    HeuristicType& heuristic,
    ParamType<State> state,
    ProgressReport progress,
    double max_time) -> std::unique_ptr<PolicyType>
{
    return algorithm->compute_policy(mdp, heuristic, state, progress, max_time);
}

template <typename State, typename Action>
void AlgorithmAdaptor<State, Action>::print_statistics(std::ostream& out) const
{
    return algorithm->print_statistics(out);
}

} // namespace probfd::solvers