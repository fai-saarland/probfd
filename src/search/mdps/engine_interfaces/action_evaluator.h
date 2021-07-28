#pragma once

#include "../types.h"
#include "../value_type.h"

namespace probabilistic {

/**
 * @brief Function object used to specify the reward gained by applying an
 * action in a given source state.
 *
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename Action>
struct ActionRewardFunction {
    virtual ~ActionRewardFunction() = default;

    /**
     * Returns the reward gained by applying \p action in the state represented
     * * by \p state_id.
     */
    value_type::value_t operator()(StateID state_id, Action action)
    {
        return this->evaluate(state_id, action);
    }

protected:
    virtual value_type::value_t evaluate(StateID state_id, Action action) = 0;
};

} // namespace probabilistic
