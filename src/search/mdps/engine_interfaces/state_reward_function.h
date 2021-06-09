#ifndef __STATE_REWARD_FUNCTION_H__
#define __STATE_REWARD_FUNCTION_H__

#include "../evaluation_result.h"

namespace probabilistic {

template<typename State>
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

}

#endif // __STATE_REWARD_FUNCTION_H__