#pragma once

#include "../state_evaluator.h"

namespace probabilistic {

/**
 * @brief Returns a constant estimate for each state.
 */
template <typename State>
class ConstantEvaluator : public StateEvaluator<State> {
public:
    /**
     * @brief Construct with constant estimate \p value .
     */
    ConstantEvaluator::ConstantEvaluator(value_type::value_t value)
        : value_(value)
    {
    }

protected:
    virtual EvaluationResult evaluate(const State& state) const override
    {
        return EvaluationResult(false, value_);
    }

    const value_type::value_t value_;
};

} // namespace probabilistic
