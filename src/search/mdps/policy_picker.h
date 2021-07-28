#pragma once

#include "engine_interfaces/policy_picker.h"
#include "heuristic_search_interfaceable.h"
#include "probabilistic_operator.h"

class GlobalState;

namespace probabilistic {

template <>
struct PolicyPicker<const ProbabilisticOperator*>
    : public HeuristicSearchInterfaceable {
public:
    int operator()(
        const StateID& state,
        const ActionID& prev_policy,
        const std::vector<const ProbabilisticOperator*>& action_choices,
        const std::vector<Distribution<StateID>>& successors);

protected:
    virtual int pick(
        const StateID& state,
        const ActionID& prev_policy,
        const std::vector<const ProbabilisticOperator*>& action_choices,
        const std::vector<Distribution<StateID>>& successors) = 0;
};

using ProbabilisticOperatorPolicyPicker =
    PolicyPicker<const ProbabilisticOperator*>;

} // namespace probabilistic
