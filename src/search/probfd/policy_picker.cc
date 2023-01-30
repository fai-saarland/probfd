#include "probfd/policy_picker.h"

#include "plugin.h"

#include <cassert>

namespace probfd {
namespace engine_interfaces {

int ProbabilisticOperatorPolicyPicker::operator()(
    const StateID& state,
    const ActionID& prev_policy,
    const std::vector<OperatorID>& action_choices,
    const std::vector<Distribution<StateID>>& successors)
{
    return pick(state, prev_policy, action_choices, successors);
}

} // namespace engine_interfaces

static PluginTypePlugin<ProbabilisticOperatorPolicyPicker>
    _plugin_type("PolicyPicker", "");

} // namespace probfd
