#include "probfd/successor_sort.h"

#include "global_state.h"
#include "plugin.h"

#include <cassert>

namespace probfd {
namespace engine_interfaces {

void SuccessorSorting<const ProbabilisticOperator*>::operator()(
    const StateID& state,
    const std::vector<const ProbabilisticOperator*>& action_choices,
    std::vector<Distribution<StateID>>& successors)
{
    return sort(state, action_choices, successors);
}

} // namespace engine_interfaces

static PluginTypePlugin<ProbabilisticOperatorSuccessorSorting>
    _plugin_type("ProbabilisticOperatorSuccessorSorting", "");

} // namespace probfd
