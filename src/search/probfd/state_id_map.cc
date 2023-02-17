#include "probfd/state_id_map.h"

#include "state_registry.h"

namespace probfd {
namespace engine_interfaces {

StateIDMap<State>::StateIDMap(StateRegistry* reg)
    : reg_(reg)
{
}

StateID StateIDMap<State>::get_state_id(const State& state)
{
    return state.get_id();
}

State StateIDMap<State>::get_state(StateID state_id)
{
    return reg_->lookup_state(::StateID(state_id));
}

} // namespace engine_interfaces
} // namespace probfd
