#ifndef MDPS_OPEN_LIST_H
#define MDPS_OPEN_LIST_H

#include "probfd/engine_interfaces/open_list.h"
#include "probfd/engine_interfaces/state_id_map.h"

#include "../task_proxy.h"

namespace probfd {
namespace engine_interfaces {

template <>
class OpenList<OperatorID> {
public:
    virtual ~OpenList() = default;
    bool empty() const;

    virtual StateID pop() = 0;
    virtual void push(const StateID& state_id) = 0;
    virtual void push(
        const StateID& parent,
        OperatorID op,
        const value_type::value_t& prob,
        const StateID& state_id);

    virtual unsigned size() const = 0;
    virtual void clear() = 0;

    void set_state_id_map(StateIDMap<State>* state_id_map);

protected:
    State lookup_state(const StateID& state_id);

private:
    StateIDMap<State>* state_id_map_ = nullptr;
};

} // namespace engine_interfaces

using GlobalStateOpenList = engine_interfaces::OpenList<OperatorID>;

} // namespace probfd

#endif // __OPEN_LIST_H__