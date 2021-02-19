#pragma once

#include "../evaluation_result.h"

namespace probabilistic {

template<typename State>
struct StateEvaluator {
    EvaluationResult operator()(const State& state);
};

} // namespace probabilistic

