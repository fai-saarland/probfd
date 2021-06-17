#pragma once

#include "../state_evaluator.h"

namespace probabilistic {

/**
 * @brief Returns a constant estimate for each state.
 */
class ConstantEvaluator : public GlobalStateEvaluator {
public:
    /**
     * @brief Construct with constant estimate \p value .
     */
    explicit ConstantEvaluator(value_type::value_t value);

protected:
    virtual EvaluationResult evaluate(const GlobalState& state) override;

    const value_type::value_t value_;
};

} // namespace probabilistic
