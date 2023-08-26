#ifndef PROBFD_ALGORITHMS_OPEN_LIST_H
#define PROBFD_ALGORITHMS_OPEN_LIST_H

#include "probfd/types.h"
#include "probfd/value_type.h"

namespace probfd {
namespace algorithms {

/**
 * @brief An interface for open lists used during search algorithms.
 *
 * An open lists is be used to specify the order in which states are expanded
 * by a search algorithm.
 *
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename Action>
class OpenList {
public:
    virtual ~OpenList() = default;

    /// Gets the current number f states in the open list.
    virtual unsigned size() const = 0;

    /// Offers a new state to the open list.
    virtual void push(StateID state_id) = 0;

    /// Extract the next state from the open list.
    virtual StateID pop() = 0;

    /// Clears the open list.
    virtual void clear() = 0;

    /// Checks if the open list is empty.
    virtual bool empty() const { return size() == 0; }

    /// Offers a new state to the open list that is the target of a transition.
    virtual void push(StateID, Action, value_t, StateID state_id)
    {
        push(state_id);
    }
};

} // namespace algorithms
} // namespace probfd

#endif // __OPEN_LIST_H__