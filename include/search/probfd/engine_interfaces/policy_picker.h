#ifndef PROBFD_ENGINE_INTERFACES_POLICY_PICKER_H
#define PROBFD_ENGINE_INTERFACES_POLICY_PICKER_H

#include "probfd/engine_interfaces/state_space.h"

#include "probfd/distribution.h"
#include "probfd/types.h"

namespace probfd {
namespace engine_interfaces {

class HeuristicSearchInterface;

/**
 * @brief Interface specifying a strategy to choose a greedy action for a state
 * in case of ties.
 *
 * This interface is used by heuristic search algorithms which maintain a
 * greedy policy during search. If there are multiple greedy actions in a state
 * after a value update has been performed, an implementation of this interface
 * breaks the ties by choosing one of the candidate actions.
 *
 * Users of this interface must implement the protected method
 * `pick(StateID, ActionID, const std::vector<Action>&, const
 * std::vector<Distribution<StateID>>&)`.
 *
 * Example
 * =======
 *
 * ```
class AlwaysFirstPicker : public PolicyPicker<const ProbabilisticOperator*> {
protected:
    int pick(
        StateID state_id,
        ActionID previous_greedy_id,
        const std::vector<const ProbabilisticOperator*>& greedy_actions,
        const std::vector<Distribution<StateID>>& candidate_successors) override
    {
        return 0; // Always choose the first greedy action in the list
    }
};
 * ```
 *
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename State, typename Action>
class PolicyPicker {
public:
    virtual ~PolicyPicker() = default;

    /**
     * @brief Select a greedy action from multiple candidates.
     *
     * @param state_id - The state id of the state to choose a greedy
     * action for
     * @param previous_greedy_id - The action id of the previous greedy action
     * for the state
     * @param greedy_action_candidates - A list of greedy actions to choose from
     * @param candidate_successors - The successor distributions for each of the
     * greedy actions
     *
     * @return An integer specifying the list index of the selected action.
     */
    virtual int pick_index(
        StateSpace<State, Action>& state_space,
        StateID state_id,
        ActionID previous_greedy_id,
        const std::vector<Action>& greedy_action_candidates,
        const std::vector<Distribution<StateID>>& candidate_successors,
        HeuristicSearchInterface& hs_interface) = 0;

    virtual void print_statistics(std::ostream&) {}
};

} // namespace engine_interfaces
} // namespace probfd

class State;
class OperatorID;

namespace probfd {

using TaskPolicyPicker = engine_interfaces::PolicyPicker<State, OperatorID>;

} // namespace probfd

#endif // __POLICY_PICKER_H__