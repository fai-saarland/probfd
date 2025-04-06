#ifndef PROBFD_HEURISTICS_CONSTANT_HEURISTIC_H
#define PROBFD_HEURISTICS_CONSTANT_HEURISTIC_H

#include "probfd/task_utils/task_properties.h"

#include "probfd/cost_function.h"
#include "probfd/heuristic.h"
#include "probfd/task_heuristic_factory.h"
#include "probfd/task_proxy.h"
#include "probfd/type_traits.h"
#include "probfd/value_type.h"

namespace probfd::heuristics {

/**
 * @brief Returns a constant estimate for each state.
 */
template <typename State>
class ConstantEvaluator : public Heuristic<State> {
    const value_t value_;

public:
    /**
     * @brief Construct with constant estimate \p value .
     */
    explicit ConstantEvaluator(value_t value)
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
class BlindEvaluator : public ConstantEvaluator<State> {
public:
    /**
     * @brief Construct with constant estimate \p value .
     */
    BlindEvaluator(
        const ProbabilisticOperatorsProxy& operators,
        const FDRCostFunction& cost_function)
        : ConstantEvaluator<State>(
              task_properties::get_min_operator_cost(operators) >= 0_vt
                  ? std::min(
                        cost_function.get_goal_termination_cost(),
                        cost_function.get_non_goal_termination_cost())
                  : -INFINITE_VALUE)
    {
    }
};

class BlindEvaluatorFactory : public TaskHeuristicFactory {
public:
    std::unique_ptr<FDREvaluator> create_heuristic(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function) override;
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_CONSTANT_HEURISTIC_H