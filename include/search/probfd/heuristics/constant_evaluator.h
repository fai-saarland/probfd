#ifndef PROBFD_HEURISTICS_CONSTANT_EVALUATOR_H
#define PROBFD_HEURISTICS_CONSTANT_EVALUATOR_H

#include "probfd/evaluator.h"

namespace probfd {
namespace heuristics {

/**
 * @brief Returns a constant estimate for each state.
 */
template <typename State>
class ConstantEvaluator : public Evaluator<State> {
    const value_t value_;

public:
    /**
     * @brief Construct with constant estimate \p value .
     */
    explicit ConstantEvaluator(value_t value)
        : value_(value)
    {
    }

    virtual EvaluationResult evaluate(param_type<State>) const override
    {
        return EvaluationResult(false, value_);
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
    BlindEvaluator()
        : ConstantEvaluator<State>(0_vt)
    {
    }
};

} // namespace heuristics
} // namespace probfd

#endif // __CONSTANT_EVALUATOR_H__