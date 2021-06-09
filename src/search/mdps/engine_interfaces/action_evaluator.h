#pragma once

#include "../types.h"
#include "../value_type.h"

namespace probabilistic {

template<typename Action>
struct ActionRewardFunction {
    virtual ~ActionRewardFunction() = default;

    value_type::value_t operator()(StateID state_id, Action action)
    {
        return this->evaluate(state_id, action);
    }

protected:
    virtual value_type::value_t
    evaluate(StateID state_id, Action action) = 0;
};

} // namespace probabilistic

