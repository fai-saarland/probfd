#pragma once

#include "../types.h"

#include <vector>

namespace probabilistic {

template<typename Action>
struct ApplicableActionsGenerator {
    void operator()(const StateID& state, std::vector<Action>& result);
};

} // namespace probabilistic

