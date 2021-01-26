#pragma once

#include "../types.h"

namespace probabilistic {

template<typename Action>
struct ActionIDMap {
    ActionID get_action_id(const StateID& state_id, const Action& action);
    Action get_action(const StateID& state_id, const ActionID& action_id);
};

} // namespace probabilistic

