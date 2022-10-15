#include "probfd/state_id_map.h"

#include "state_registry.h"

namespace probfd {
namespace engine_interfaces {

StateIDMap<GlobalState>::StateIDMap(StateRegistry* reg)
    : reg_(reg)
{
}

StateID StateIDMap<GlobalState>::get_state_id(const GlobalState& state)
{
    return state.get_id().hash();
}

GlobalState StateIDMap<GlobalState>::get_state(const StateID& state_id)
{
    return reg_->lookup_state(::StateID(state_id));
}

} // namespace engine_interfaces
} // namespace probfd
