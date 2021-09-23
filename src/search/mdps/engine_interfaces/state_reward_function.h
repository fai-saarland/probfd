#ifndef MDPS_ENGINE_INTERACES_STATE_REWARD_FUNCTION_H
#define MDPS_ENGINE_INTERACES_STATE_REWARD_FUNCTION_H

#include "../evaluation_result.h"

namespace probabilistic {

template <typename State>
class StateRewardFunction {
public:
    virtual ~StateRewardFunction() = default;

    EvaluationResult operator()(const State& state)
    {
        return this->evaluate(state);
    }

    virtual void print_statistics() const {}

protected:
    virtual EvaluationResult evaluate(const State& state) = 0;
};

} // namespace probabilistic

#endif // __STATE_REWARD_FUNCTION_H__