#ifndef PROBFD_ENGINE_INTERFACES_TRANSITION_SORTER_H
#define PROBFD_ENGINE_INTERFACES_TRANSITION_SORTER_H

#include "probfd/types.h"

#include <vector>

namespace probfd {

template <typename>
class Distribution;

namespace engine_interfaces {

class StateProperties;

/**
 * @brief An interface used to reorder a list of transitions.
 *
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename Action>
class TransitionSorter {
public:
    virtual ~TransitionSorter() = default;

    /**
     * @brief Sorts a list of transitions.
     *
     * @param state - The common source state for all transitions
     * @param aops - The transition actions
     * @param successors - The successor distributions of the transitions. To be
     * sorted.
     * @param properties - The interface to the heuristic search algorithm.
     * Can be used to query additional information about the involved states and
     * actions.
     */
    virtual void sort(
        StateID state,
        const std::vector<Action>& aops,
        std::vector<Distribution<StateID>>& successors,
        StateProperties& properties) = 0;
};

} // namespace engine_interfaces
} // namespace probfd

class OperatorID;

namespace probfd {

/// Type alias for TransitionSorters for probabilistic planning tasks.
using TaskTransitionSorter = engine_interfaces::TransitionSorter<OperatorID>;

} // namespace probfd

#endif // __SUCCESSOR_SORTING_H__