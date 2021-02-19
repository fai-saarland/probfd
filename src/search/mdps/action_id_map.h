#pragma once

#include "engine_interfaces/action_id_map.h"
#include "probabilistic_operator.h"

#include <cassert>

namespace probabilistic {

template<>
class ActionIDMap<const ProbabilisticOperator*> {
public:
    explicit ActionIDMap(const std::vector<ProbabilisticOperator>& operators);
    explicit ActionIDMap();

    ActionID get_action_id(
        const StateID& state_id,
        const ProbabilisticOperator* const& op);

    const ProbabilisticOperator*
    get_action(const StateID& state_id, const ActionID& action_id);

private:
    const ProbabilisticOperator* first_;
#ifndef NDEBUG
    std::size_t num_operators_;
#endif
};

} // namespace probabilistic
