#pragma once

#include "../types.h"
#include "../value_type.h"

namespace probabilistic {

template<typename Action>
struct ActionEvaluator {
    value_type::value_t
    operator()(const StateID& state_id, const Action& action);
};

} // namespace probabilistic

