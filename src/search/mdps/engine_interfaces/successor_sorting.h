#pragma once

#include "../distribution.h"
#include "../types.h"

#include <vector>

namespace probabilistic {

/**
 * @brief Function object used to sort the possible successors of a transition
 * in the MDP model.
 *
 * @tparam Action - The action type of the underlying MDP model.
 *
 * @note The successor sorting operates on state IDs, so the underlying state
 * type of the MDP model is not required.
 */
template <typename Action>
struct SuccessorSorting {
    void operator()(
        const StateID& state,
        const std::vector<Action>& aops,
        std::vector<Distribution<StateID>>& successors);
};

} // namespace probabilistic
