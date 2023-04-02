#ifndef PROBFD_ENGINE_INTERFACES_STATE_SPACE_H
#define PROBFD_ENGINE_INTERFACES_STATE_SPACE_H

#include "probfd/distribution.h"
#include "probfd/type_traits.h"
#include "probfd/types.h"

#include <vector>

namespace probfd {

/// This namespace contains interfaces related to search engines.
namespace engine_interfaces {

/**
 * @brief An interface representing a Markov Decision Process (MDP) without
 * objective function.
 *
 * This interface has four responsibilities:
 * 1. The translation between states and state IDs. While states can be large
 * explicit representations, StateIDs are cheap to store and copy.
 * 2. The translation between actions and action IDs. While actions can be
 * large explicit representations, ActionIDs are cheap to store and copy.
 * 3. The computation of the set of actions applicable in a state.
 * 4. The computation of the transitions of the MDP.
 *
 * @tparam State - The state type of the MDP.
 * @tparam Action - The action type of the MDP.
 *
 * @todo The indirection via ActionIDs seems very redundant. In all current
 * implementations of this class, the action type is a reference-like type
 * anyway, since the list of operators of the underlying planning task/
 * projection etc. inducing the state space is fully stored in memory.
 * We might aswell remove the extra conversion and require that the Action
 * type is cheap to copy via the cheap_to_copy type trait.
 *
 * @todo One could also parameterize this interface with a custom ID type for
 * states. This would make it possible to specify Fast Downward's StateID type
 * as well, and the hacked conversion to our StateID type could be removed.
 * It would also enable optimizations if conversion is not needed because the
 * State type is already cheap to copy, e.g. the StateRank type in the PDB
 * implementation. In that case, the state ID conversion functions could be
 * declared inline as the identity function and compilers could optimize out
 * the call while the programming abstraction remains. The major downside of
 * this approach is that the StateID type is used frequently in the code base,
 * so the codebase might become even more infested with templates.
 */
template <typename State, typename Action>
class StateSpace {
public:
    virtual ~StateSpace() = default;

    /**
     * @brief Get the state ID for a given state.
     */
    virtual StateID get_state_id(param_type<State> state) = 0;

    /**
     * @brief Get the state mapped to a given state ID.
     */
    virtual State get_state(StateID state_id) = 0;

    /**
     * @brief Gets the action ID of a given state-action.
     */
    virtual ActionID
    get_action_id(StateID state_id, param_type<Action> action) = 0;

    /**
     * @brief Get the action for a given state and action ID.
     *
     * @param state_id - The ID of the state in which the represented action is
     * applicable.
     * @param action_id - The action ID.
     * @returns Action - The represented action.
     */
    virtual Action get_action(StateID state_id, ActionID action_id) = 0;

    /**
     * @brief Generates the applicable actions of the state.
     */
    virtual void
    generate_applicable_actions(StateID state, std::vector<Action>& result) = 0;

    /**
     * @brief Generates the successor distribution for a given state and action.
     */
    virtual void generate_action_transitions(
        StateID state,
        param_type<Action> action,
        Distribution<StateID>& result) = 0;

    /**
     * @brief Generates all applicable actions and their corresponding successor
     * distributions for a given state.
     */
    virtual void generate_all_transitions(
        StateID state,
        std::vector<Action>& aops,
        std::vector<Distribution<StateID>>& successors) = 0;
};

} // namespace engine_interfaces
} // namespace probfd

#endif