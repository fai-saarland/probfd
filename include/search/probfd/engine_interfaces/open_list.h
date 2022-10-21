#ifndef MDPS_ENGINE_INTERACES_OPEN_LIST_H
#define MDPS_ENGINE_INTERACES_OPEN_LIST_H

#include "probfd/types.h"
#include "probfd/value_type.h"

#include <deque>

namespace probfd {
namespace engine_interfaces {

/**
 * @brief Open list used by the MDP search engine.
 *
 * @tparam Action - The action type of the underlying MDP model.
 *
 * @note The open list operates on state IDs, so the underlying state type
 * of the MDP model is not required.
 *
 * @see StateIDMap
 */
template <typename Action>
class OpenList {
public:
    /**
     * Pops and returns the next state to be expanded from the open list.
     */
    StateID pop()
    {
        const StateID res = queue_.back();
        queue_.pop_back();
        return res;
    }

    /**
     * Pushes the new expansion candidate with id \p state_id onto the open
     * list.
     */
    void push(const StateID& state_id) { queue_.push_back(state_id); }

    void push(
        const StateID&,
        const Action&,
        const value_type::value_t&,
        const StateID& state_id)
    {
        queue_.push_back(state_id);
    }

    /**
     * Returns the size of the open list, i.e. the number of states currently
     * eligible for expansion.
     */
    unsigned size() const { return queue_.size(); }

    /**
     * Checks if the open list is empty.
     */
    bool empty() const { return queue_.empty(); }

    /**
     * Clears the open list.
     */
    void clear() { queue_.clear(); }

private:
    std::deque<StateID> queue_;
};

} // namespace engine_interfaces
} // namespace probfd

#endif // __OPEN_LIST_H__