#include "successor_sort.h"

#include "../global_state.h"
#include "../plugin.h"

#include <cassert>

namespace probabilistic {

void
ProbabilisticOperatorSuccessorSorting::operator()(
    const StateID& state,
    const std::vector<const ProbabilisticOperator*>& action_choices,
    std::vector<Distribution<StateID>>& successors)
{
    return sort(state, action_choices, successors);
}

static PluginTypePlugin<ProbabilisticOperatorSuccessorSorting>
    _plugin_type("ProbabilisticOperatorSuccessorSorting", "");

} // namespace probabilistic
