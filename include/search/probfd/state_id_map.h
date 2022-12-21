#ifndef MDPS_STATE_ID_MAP_H
#define MDPS_STATE_ID_MAP_H

#include "probfd/engine_interfaces/state_id_map.h"

#include "legacy/global_state.h"

namespace legacy {
class StateRegistry;
}

namespace probfd {
namespace engine_interfaces {

template <>
class StateIDMap<legacy::GlobalState> {
public:
    explicit StateIDMap(legacy::StateRegistry* reg);
    StateID get_state_id(const legacy::GlobalState& state);
    legacy::GlobalState get_state(const StateID& state_id);

private:
    legacy::StateRegistry* reg_;
};

} // namespace engine_interfaces
} // namespace probfd

#endif // __STATE_ID_MAP_H__