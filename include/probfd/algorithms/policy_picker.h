#ifndef PROBFD_ALGORITHMS_POLICY_PICKER_H
#define PROBFD_ALGORITHMS_POLICY_PICKER_H

#include "probfd/types.h"

#include <optional>
#include <ostream>
#include <vector>

// Forward Declarations
namespace probfd {
template <typename>
class Distribution;
template <typename>
struct Transition;
template <typename, typename>
class MDP;
} // namespace probfd

namespace probfd::algorithms {
class StateProperties;
}

namespace probfd::algorithms {
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
class AlwaysFirstPicker : public PolicyPicker<OperatorID> {
protected:
    int pick_index(
        MDP<State, OperatorID>&,
        StateID,
        OperatorID,
        const std::vector<OperatorID>&,
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
     * @brief Selects a greedy transition from multiple candidates by returning
     * its index in the candidate list.
     *
     * @param mdp - The MDP in which the search is performed.
     * @param incumbent_greedy - The incumbent greedy action.
     * @param greedy_transitions - A list of candidate greedy transitions.
     * @param properties - The interface to the heuristic search algorithm.
     * Can be used to query additional information about the involved states and
     * actions.
     */
    virtual int pick_index(
        MDP<State, Action>& mdp,
        std::optional<Action> incumbent_greedy,
        const std::vector<Transition<Action>>& greedy_transitions,
        StateProperties& properties) = 0;

    /**
     * @brief Prints statistics to an output stream, e.g. the number of queries
     * made to the interface.
     */
    virtual void print_statistics(std::ostream&) {}
};

} // namespace probfd::algorithms

#endif // PROBFD_ALGORITHMS_POLICY_PICKER_H