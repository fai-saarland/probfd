#include "open_list.h"

#include "../../plugin.h"

namespace probabilistic {
namespace algorithms {

bool
OpenList<GlobalState, const ProbabilisticOperator*>::empty() const
{
    return this->size() == 0;
}

void
OpenList<GlobalState, const ProbabilisticOperator*>::push(
    const StateID& state_id,
    const GlobalState&,
    const ProbabilisticOperator*,
    const value_type::value_t&,
    const GlobalState& state)
{
    this->push(state_id, state);
}

} // namespace algorithms

namespace open_lists {

static PluginTypePlugin<GlobalStateOpenList>
    _plugin_type("GlobalStateOpenList", "");

} // namespace open_lists
} // namespace probabilistic
