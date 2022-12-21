#include "probfd/state_id_map.h"

#include "legacy/state_registry.h"

namespace probfd {
namespace engine_interfaces {

StateIDMap<legacy::GlobalState>::StateIDMap(legacy::StateRegistry* reg)
    : reg_(reg)
{
}

StateID
StateIDMap<legacy::GlobalState>::get_state_id(const legacy::GlobalState& state)
{
    return state.get_id().hash();
}

legacy::GlobalState
StateIDMap<legacy::GlobalState>::get_state(const StateID& state_id)
{
    return reg_->lookup_state(legacy::StateID(state_id));
}

} // namespace engine_interfaces
} // namespace probfd
