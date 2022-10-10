#ifndef MDPS_HEURISTICS_CONSTANT_EVALUATOR_H
#define MDPS_HEURISTICS_CONSTANT_EVALUATOR_H

#include "../state_evaluator.h"

namespace probfd {
namespace heuristics {

/**
 * @brief Returns a constant estimate for each state.
 */
template <typename State>
class ConstantEvaluator : public StateEvaluator<State> {
public:
    /**
     * @brief Construct with constant estimate \p value .
     */
    ConstantEvaluator(value_type::value_t value)
        : value_(value)
    {
    }

protected:
    virtual EvaluationResult evaluate(const State&) const override
    {
        return EvaluationResult(false, value_);
    }

    const value_type::value_t value_;
};

} // namespace heuristics
} // namespace probfd

#endif // __CONSTANT_EVALUATOR_H__