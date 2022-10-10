#include "policy_picker.h"

#include "../global_state.h"
#include "../plugin.h"

#include <cassert>

namespace probfd {

int ProbabilisticOperatorPolicyPicker::operator()(
    const StateID& state,
    const ActionID& prev_policy,
    const std::vector<const ProbabilisticOperator*>& action_choices,
    const std::vector<Distribution<StateID>>& successors)
{
    return pick(state, prev_policy, action_choices, successors);
}

static PluginTypePlugin<ProbabilisticOperatorPolicyPicker>
    _plugin_type("PolicyPicker", "");

} // namespace probfd
