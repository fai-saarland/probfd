#pragma once

#include "../global_state.h"
#include "engine_interfaces/state_evaluator.h"

namespace probabilistic {

template<>
class StateEvaluator<GlobalState> {
public:
    virtual ~StateEvaluator() = default;
    EvaluationResult operator()(const GlobalState& state);

    virtual void print_statistics() const {}

protected:
    virtual EvaluationResult evaluate(const GlobalState& state) = 0;
};

using GlobalStateEvaluator = StateEvaluator<GlobalState>;

} // namespace probabilistic
