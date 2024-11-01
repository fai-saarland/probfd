#ifndef PROBFD_ALGORITHMS_TRANSITION_SORTER_H
#define PROBFD_ALGORITHMS_TRANSITION_SORTER_H

#include "probfd/type_traits.h"
#include "probfd/types.h"

#include <vector>

// Forward Declarations
namespace probfd {
template <typename>
class Distribution;
}

namespace probfd::algorithms {
class StateProperties;
}

namespace probfd::algorithms {

/**
 * @brief An interface used to reorder a list of transitions.
 *
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename State, typename Action>
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
        param_type<State> state,
        const std::vector<Action>& aops,
        std::vector<Distribution<StateID>>& successors,
        StateProperties& properties) = 0;
};

} // namespace probfd::algorithms

#endif // PROBFD_ALGORITHMS_TRANSITION_SORTER_H