#pragma once

#include "../types.h"

namespace probabilistic {

template<typename State>
struct StateIDMap {
    StateID get_state_id(const State& state);
    State get_state(const StateID& state_id);
};

} // namespace probabilistic

