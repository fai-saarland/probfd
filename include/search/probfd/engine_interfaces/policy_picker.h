#ifndef PROBFD_ENGINE_INTERFACES_POLICY_PICKER_H
#define PROBFD_ENGINE_INTERFACES_POLICY_PICKER_H

#include "probfd/engine_interfaces/state_space.h"

#include "probfd/types.h"

#include <optional>

namespace probfd {

template <typename>
class Distribution;

namespace engine_interfaces {

class StateProperties;

/**
 * @brief An strategy interface used to choose break ties between multiple
 * greedy actions for a state.
 *
 * This interface is used by heuristic search algorithms which maintain a
 * greedy policy during search. If there are multiple greedy actions in a state
 * after a value update has been performed and the policy needs to be adjusted,
 * an implementation of this interface breaks the ties by choosing one of the
 * candidate greedy actions.
 *
 * Example
 * =======
 *
 * ```
class AlwaysFirstPicker : public PolicyPicker<const ProbabilisticOperator*> {
protected:
    int pick_index(
        StateSpace<State, Action>&,
        StateID,
        Action,
        const std::vector<Action>&,
        const std::vector<Distribution<StateID>>&,
        StateProperties&) override
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
     * @brief Selects a greedy action from multiple candidates by returning its
     * index in the candidate list.
     *
     * @param state_space - The state space in which the search is performed.
     * @param state_id - The state for which a greedy action is chosen.
     * @param previous_greedy_id - The ID of the previous greedy action.
     * @param greedy_action_candidates - A list of candidate greedy actions.
     * @param candidate_successors - The successor distributions for each of the
     * candidate greedy actions.
     * @param properties - The interface to the heuristic search algorithm.
     * Can be used to query additional information about the involved states and
     * actions.
     */
    virtual int pick_index(
        StateSpace<State, Action>& state_space,
        StateID state_id,
        std::optional<Action> previous_greedy,
        const std::vector<Action>& greedy_action_candidates,
        const std::vector<Distribution<StateID>>& candidate_successors,
        StateProperties& properties) = 0;

    /**
     * @brief Prints statistics to an output stream, e.g. the number of queries
     * made to the interface.
     */
    virtual void print_statistics(std::ostream&) {}
};

} // namespace engine_interfaces
} // namespace probfd

#endif // __POLICY_PICKER_H__