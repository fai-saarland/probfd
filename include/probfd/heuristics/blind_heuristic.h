#ifndef PROBFD_HEURISTICS_BLIND_HEURISTIC_H
#define PROBFD_HEURISTICS_BLIND_HEURISTIC_H

#include "probfd/heuristics/constant_heuristic.h"

#include "probfd/probabilistic_operator_cost_function_fwd.h"
#include "probfd/task_heuristic_factory.h"

namespace probfd {
class ProbabilisticOperatorSpace;
class TerminationCosts;
} // namespace probfd

namespace probfd::heuristics {

/**
 * @brief A blind heuristic that always returns the minimum temrination cost
 * if operator costs are non-negative and -inf otherwise.
 */
template <typename State>
class BlindHeuristic : public ConstantHeuristic<State> {
public:
    BlindHeuristic(
        const ProbabilisticOperatorSpace& operators,
        const ProbabilisticOperatorCostFunction& cost_function,
        const TerminationCosts& termination_costs);

    explicit BlindHeuristic(const ProbabilisticTaskTuple& task);
};

class BlindHeuristicFactory : public TaskHeuristicFactory {
public:
    std::unique_ptr<FDRHeuristic>
    create_object(const SharedProbabilisticTask& task) override;
};

} // namespace probfd::heuristics

#include "probfd/task_utils/task_properties.h"

#include "probfd/termination_costs.h"
#include "probfd/value_type.h"

#include "downward/operator_cost_function.h"

namespace probfd::heuristics {

template <typename State>
BlindHeuristic<State>::BlindHeuristic(
    const ProbabilisticOperatorSpace& operators,
    const ProbabilisticOperatorCostFunction& cost_function,
    const TerminationCosts& termination_costs)
    : ConstantHeuristic<State>(task_properties::get_cost_lower_bound(
          operators,
          cost_function,
          termination_costs))
{
}

template <typename State>
BlindHeuristic<State>::BlindHeuristic(const ProbabilisticTaskTuple& task)
    : BlindHeuristic(
          get_operators(task),
          get_cost_function(task),
          get_termination_costs(task))
{
}

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_BLIND_HEURISTIC_H