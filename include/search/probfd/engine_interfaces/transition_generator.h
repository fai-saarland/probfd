#ifndef MDPS_ENGINE_INTERFACES_TRANSITION_GENERATOR_H
#define MDPS_ENGINE_INTERFACES_TRANSITION_GENERATOR_H

#include "probfd/distribution.h"
#include "probfd/types.h"

#include <vector>

namespace probfd {
namespace engine_interfaces {

/**
 * @brief Function object representing the transition relation of the MDP.
 * Is responsible for generating the applicable actions of a state and the
 * successor distribution for a given applicable action. Can also generate all
 * applicable actions and successor distributions at once for a given state.
 *
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename Action>
struct TransitionGenerator {
    /**
     * Generates all applicable actions of the state \p state and outputs them
     * in \p result.
     */
    void
    generate_applicable_actions(StateID state, std::vector<Action>& result);

    void generate_action_transitions(
        StateID state,
        const Action& action,
        Distribution<StateID>& result);

    void generate_all_transitions(
        StateID state,
        std::vector<Action>& aops,
        std::vector<Distribution<StateID>>& successors);
};

} // namespace engine_interfaces
} // namespace probfd

#endif // __TRANSITION_GENERATOR_H__