#pragma once

#include "../evaluation_result.h"

namespace probabilistic {

/**
 * @brief Function object specifying the immediate reward received in a target
 * state, independent of the source state and action taken.
 * 
 * @tparam State - The state type of the underlying MDP model.
 * 
 * @see EvaluationResult
 */
template<typename State>
class StateEvaluator {
public:
    virtual ~StateEvaluator() = default;

    EvaluationResult operator()(const State& state)
    {
        return this->evaluate(state);
    }

    virtual void print_statistics() const {}

protected:
    virtual EvaluationResult evaluate(const State& state) = 0;
};

template<typename State>
class ConstantValueInitializer : public StateEvaluator<State>
{
    value_type::value_t init_value_;

public:
    ConstantValueInitializer(value_type::value_t init_value)
        : init_value_(init_value)
    {
    }

    EvaluationResult evaluate(const State&) override
    {
        return EvaluationResult(false, init_value_);
    }
};

} // namespace probabilistic

