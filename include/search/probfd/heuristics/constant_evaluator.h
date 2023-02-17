#ifndef PROBFD_HEURISTICS_CONSTANT_EVALUATOR_H
#define PROBFD_HEURISTICS_CONSTANT_EVALUATOR_H

#include "probfd/engine_interfaces/state_evaluator.h"

namespace probfd {
namespace heuristics {

/**
 * @brief Returns a constant estimate for each state.
 */
template <typename State>
class ConstantEvaluator : public engine_interfaces::StateEvaluator<State> {
    const value_t value_;

public:
    /**
     * @brief Construct with constant estimate \p value .
     */
    explicit ConstantEvaluator(value_t value)
        : value_(value)
    {
    }

    virtual EvaluationResult evaluate(const State&) const override
    {
        return EvaluationResult(false, value_);
    }
};

} // namespace heuristics
} // namespace probfd

#endif // __CONSTANT_EVALUATOR_H__