#pragma once

#include "../distribution.h"
#include "../types.h"

#include <vector>

namespace probabilistic {

template<typename Action>
struct TransitionGenerator {
    void operator()(
        const StateID& state,
        const Action& action,
        Distribution<StateID>& result);
    void operator()(
        const StateID& state,
        std::vector<Action>& aops,
        std::vector<Distribution<StateID>>& successors);
};

} // namespace probabilistic
