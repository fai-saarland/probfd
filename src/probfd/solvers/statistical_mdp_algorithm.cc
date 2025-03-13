#include "probfd/solvers/statistical_mdp_algorithm.h"

#include "probfd/progress_report.h"
#include "probfd/mdp_algorithm.h"

#include "downward/operator_id.h"
#include "downward/task_proxy.h"

#include <iostream>

namespace probfd::solvers {

AlgorithmAdaptor::AlgorithmAdaptor(std::unique_ptr<FDRMDPAlgorithm> algorithm)
    : algorithm(std::move(algorithm))
{
}

AlgorithmAdaptor::~AlgorithmAdaptor() = default;

auto AlgorithmAdaptor::compute_policy(
    MDPType& mdp,
    HeuristicType& heuristic,
    ParamType<State> state,
    ProgressReport progress,
    double max_time) -> std::unique_ptr<PolicyType>
{
    return algorithm->compute_policy(mdp, heuristic, state, progress, max_time);
}

void AlgorithmAdaptor::print_statistics(std::ostream& out) const
{
    return algorithm->print_statistics(out);
}

} // namespace probfd::solvers