#pragma once

#include "../types.h"

#include <deque>

namespace probabilistic {

enum class DeadEndIdentificationLevel {
    Disabled,
    Policy,
    Visited,
    Complete,
};

template<typename State, typename Action>
class DeadEndListener {
public:
    using is_default_implementation = std::true_type;

    bool operator()(const StateID&) { return false; }

    bool operator()(
        std::deque<StateID>::const_iterator,
        std::deque<StateID>::const_iterator)
    {
        return false;
    }
};

} // namespace probabilistic
