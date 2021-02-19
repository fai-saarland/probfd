#pragma once

#include "../distribution.h"
#include "../types.h"

#include <vector>

namespace probabilistic {

template<typename Action>
struct SuccessorSorting {
    void operator()(
        const StateID& state,
        const std::vector<Action>& aops,
        std::vector<Distribution<StateID>>& successors);
};

} // namespace probabilistic
