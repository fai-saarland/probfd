#pragma once

#include "../distribution.h"
#include "../types.h"

namespace probabilistic {

template <typename Action>
struct PolicyPicker {
    /// Tags the default implementation.
    using is_default_implementation = std::true_type;

    int operator()(
        const StateID&,
        const ActionID&,
        const std::vector<Action>&,
        const std::vector<Distribution<StateID>>&)
    {
        return 0;
    }
};

} // namespace probabilistic
