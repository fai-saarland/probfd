#ifndef PROBFD_ENGINE_INTERFACES_OPEN_LIST_H
#define PROBFD_ENGINE_INTERFACES_OPEN_LIST_H

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
    virtual ~OpenList() = default;

    virtual unsigned size() const = 0;

    virtual void push(const StateID& state_id) = 0;
    virtual StateID pop() = 0;

    virtual void clear() = 0;

    virtual bool empty() const { return size() == 0; }

    virtual void push(
        const StateID&,
        const Action&,
        const value_t&,
        const StateID& state_id)
    {
        push(state_id);
    }
};

} // namespace engine_interfaces
} // namespace probfd

class OperatorID;

namespace probfd {
using TaskOpenList = engine_interfaces::OpenList<OperatorID>;
}

#endif // __OPEN_LIST_H__