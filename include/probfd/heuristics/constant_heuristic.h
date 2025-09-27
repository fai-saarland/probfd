#ifndef PROBFD_HEURISTICS_CONSTANT_HEURISTIC_H
#define PROBFD_HEURISTICS_CONSTANT_HEURISTIC_H

#include "probfd/task_utils/task_properties.h"

#include "probfd/cost_function.h"
#include "probfd/heuristic.h"
#include "probfd/task_heuristic_factory.h"
#include "probfd/termination_costs.h"
#include "probfd/type_traits.h"
#include "probfd/value_type.h"

namespace probfd::heuristics {

/**
 * @brief Returns a constant estimate for each state.
 */
template <typename State>
class ConstantHeuristic : public Heuristic<State> {
    const value_t value_;

public:
    /**
     * @brief Construct with constant estimate \p value .
     */
    explicit ConstantHeuristic(value_t value)
        : value_(value)
    {
    }

    [[nodiscard]]
    value_t evaluate(ParamType<State>) const override
    {
        return value_;
    }
};

/**
 * @brief Returns an estimate of zero for each state.
 */
template <typename State>
class BlindHeuristic : public ConstantHeuristic<State> {
public:
    /**
     * @brief Construct with constant estimate \p value .
     */
    BlindHeuristic(
        const ProbabilisticOperatorSpace& operators,
        const downward::OperatorCostFunction<value_t>& cost_function,
        const TerminationCosts& termination_costs)
        : ConstantHeuristic<State>(task_properties::get_cost_lower_bound(
              operators,
              cost_function,
              termination_costs))
    {
    }
};

class BlindHeuristicFactory : public TaskHeuristicFactory {
public:
    std::unique_ptr<FDRHeuristic>
    create_object(const SharedProbabilisticTask& task) override;
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_CONSTANT_HEURISTIC_H