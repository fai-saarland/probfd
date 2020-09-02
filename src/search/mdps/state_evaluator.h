#pragma once

#include "../global_state.h"
#include "algorithms/types_common.h"

namespace probabilistic {
namespace algorithms {

template<>
class StateEvaluationFunction<GlobalState> {
public:
    virtual ~StateEvaluationFunction() = default;
    EvaluationResult operator()(const GlobalState& state);

protected:
    virtual EvaluationResult evaluate(const GlobalState& state) = 0;
};

} // namespace algorithms

using GlobalStateEvaluator = algorithms::StateEvaluationFunction<GlobalState>;

} // namespace probabilistic
