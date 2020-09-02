#pragma once

#include "../state_evaluator.h"

namespace probabilistic {
namespace const_value_evaluator {

class ConstantEvaluator : public GlobalStateEvaluator {
public:
    explicit ConstantEvaluator(value_type::value_t value);

protected:
    virtual EvaluationResult evaluate(const GlobalState& state) override;

    const value_type::value_t value_;
};

} // namespace const_value_evaluator
} // namespace probabilistic
