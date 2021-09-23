#ifndef MDPS_ENGINE_INTERACES_APPLICABLE_ACTIONS_GENERATOR_H
#define MDPS_ENGINE_INTERACES_APPLICABLE_ACTIONS_GENERATOR_H

#include "../types.h"

#include <vector>

namespace probabilistic {

/**
 * @brief Function object that generates all applicable actions in a given
 * state.
 *
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename Action>
struct ApplicableActionsGenerator {
    /**
     * Generates all applicable actions of the state \p state and outputs them
     * in \p result.
     */
    void operator()(const StateID& state, std::vector<Action>& result);
};

} // namespace probabilistic

#endif // __APPLICABLE_ACTIONS_GENERATOR_H__