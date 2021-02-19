#pragma once

#include "../global_state.h"
#include "engine_interfaces/state_id_map.h"

class StateRegistry;

namespace probabilistic {

template<>
class StateIDMap<GlobalState> {
public:
    explicit StateIDMap(StateRegistry* reg);
    StateID get_state_id(const GlobalState& state);
    GlobalState get_state(const StateID& state_id);

private:
    StateRegistry* reg_;
};

} // namespace probabilistic

