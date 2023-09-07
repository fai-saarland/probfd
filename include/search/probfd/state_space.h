#ifndef PROBFD_STATE_SPACE_H
#define PROBFD_STATE_SPACE_H

#include "probfd/type_traits.h"
#include "probfd/types.h"

#include <vector>

namespace probfd {

template <typename>
class Distribution;

template <typename>
struct Transition;

/**
 * @brief An interface representing a Markov Decision Process (MDP) without
 * objective function.
 *
 * This interface has four responsibilities:
 * 1. The translation between states and state IDs. While states can be large
 * explicit representations, StateIDs are cheap to store and copy.
 * 2. The computation of the set of actions applicable in a state.
 * 3. The computation of the transitions of the MDP.
 *
 * @tparam State - The state type of the MDP.
 * @tparam Action - The action type of the MDP.
 *
 * @todo One could also parameterize this interface with a custom ID type for
 * states. This would make it possible to specify Fast Downward's StateID type
 * as well, and the hacked conversion to our StateID type could be removed.
 * It would also enable optimizations if conversion is not needed because the
 * State type is already cheap to copy, e.g. the AbstractStateIndex type in the
 * PDB implementation. In that case, the state ID conversion functions could be
 * declared inline as the identity function and compilers could optimize out
 * the call while the programming abstraction remains. The major downside of
 * this approach is that the StateID type is used frequently in the code base,
 * so the codebase might become even more infested with templates.
 */
template <typename State, typename Action>
class StateSpace {
protected:
    using Transition = Transition<Action>;

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
     * @brief Generates the applicable actions of the state.
     */
    virtual void generate_applicable_actions(
        param_type<State> state,
        std::vector<Action>& result) = 0;

    /**
     * @brief Generates the successor distribution for a given state and action.
     */
    virtual void generate_action_transitions(
        param_type<State> state,
        param_type<Action> action,
        Distribution<StateID>& result) = 0;

    /**
     * @brief Generates all applicable actions and their corresponding successor
     * distributions for a given state.
     */
    virtual void generate_all_transitions(
        param_type<State> state,
        std::vector<Action>& aops,
        std::vector<Distribution<StateID>>& successors) = 0;

    /**
     * @brief Generates all applicable actions and their corresponding successor
     * distributions for a given state.
     */
    virtual void generate_all_transitions(
        param_type<State> state,
        std::vector<Transition>& transitions) = 0;
};

} // namespace probfd

#endif