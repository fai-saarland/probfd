#ifndef MDPS_ENGINE_INTERACES_TRANSITION_GENERATOR_H
#define MDPS_ENGINE_INTERACES_TRANSITION_GENERATOR_H

#include "../distribution.h"
#include "../types.h"

#include <vector>

namespace probfd {
namespace engine_interfaces {

/**
 * @brief Function object used to generate the successor distribution for the
 * given source state and applied action.
 *
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename Action>
struct TransitionGenerator {
    /**
     * Generates all applicable actions of the state \p state and outputs them
     * in \p result.
     */
    void operator()(const StateID& state, std::vector<Action>& result);

    void operator()(
        const StateID& state,
        const Action& action,
        Distribution<StateID>& result);
    void operator()(
        const StateID& state,
        std::vector<Action>& aops,
        std::vector<Distribution<StateID>>& successors);
};

} // namespace engine_interfaces
} // namespace probfd

#endif // __TRANSITION_GENERATOR_H__