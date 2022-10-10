#ifndef MDPS_ENGINE_INTERACES_POLICY_PICKER_H
#define MDPS_ENGINE_INTERACES_POLICY_PICKER_H

#include "../distribution.h"
#include "../types.h"

namespace probfd {

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
 * `pick(const StateID&, const ActionID&, const std::vector<Action>&, const
 * std::vector<Distribution<StateID>>&)`.
 *
 * Example
 * =======
 *
 * ```
class AlwaysFirstPicker : public PolicyPicker<const ProbabilisticOperator*> {
protected:
    int pick(
        const StateID& state_id,
        const ActionID& previous_greedy_id,
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
template <typename Action>
class PolicyPicker {
public:
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
    int operator()(
        const StateID& state_id,
        const ActionID& previous_greedy_id,
        const std::vector<Action>& greedy_action_candidates,
        const std::vector<Distribution<StateID>>& candidate_successors)
    {
        return this->pick(
            state_id,
            previous_greedy_id,
            greedy_action_candidates,
            candidate_successors);
    }

protected:
    int pick(
        const StateID&,
        const ActionID&,
        const std::vector<Action>&,
        const std::vector<Distribution<StateID>>&)
    {
        return 0;
    }
};

} // namespace probfd

#endif // __POLICY_PICKER_H__