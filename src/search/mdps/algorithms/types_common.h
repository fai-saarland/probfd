#pragma once

#include "../../state_id.h"
#include "../distribution.h"
#include "../evaluation_result.h"
#include "../value_type.h"

#include <vector>

namespace probabilistic {
namespace algorithms {

template<typename State>
struct StateIDMap {
    StateID operator[](const State& state);
    State operator[](const StateID& state_id);
};

template<typename State, typename Action>
struct ApplicableActionsGenerator {
    void operator()(const State& state, std::vector<Action>& result);
};

template<typename State, typename Action>
struct TransitionGenerator {
    Distribution<State> operator()(const State& state, const Action& action);
};

template<typename State>
struct StateEvaluationFunction {
    EvaluationResult operator()(const State& state);
};

template<typename State, typename Action>
struct TransitionRewardFunction {
    value_type::value_t operator()(const State& state, const Action& action);
};

} // namespace algorithms
} // namespace probabilistic

