#ifndef MDPS_OPEN_LIST_H
#define MDPS_OPEN_LIST_H

#include "probfd/engine_interfaces/open_list.h"
#include "probfd/engine_interfaces/state_id_map.h"

#include "probfd/probabilistic_operator.h"

#include "legacy/global_state.h"

namespace probfd {
namespace engine_interfaces {

template <>
class OpenList<const ProbabilisticOperator*> {
public:
    virtual ~OpenList() = default;
    bool empty() const;

    virtual StateID pop() = 0;
    virtual void push(const StateID& state_id) = 0;
    virtual void push(
        const StateID& parent,
        const ProbabilisticOperator* op,
        const value_type::value_t& prob,
        const StateID& state_id);

    virtual unsigned size() const = 0;
    virtual void clear() = 0;

    void set_state_id_map(StateIDMap<legacy::GlobalState>* state_id_map);

protected:
    legacy::GlobalState lookup_state(const StateID& state_id);

private:
    StateIDMap<legacy::GlobalState>* state_id_map_ = nullptr;
};

} // namespace engine_interfaces

using GlobalStateOpenList =
    engine_interfaces::OpenList<const ProbabilisticOperator*>;

} // namespace probfd

#endif // __OPEN_LIST_H__