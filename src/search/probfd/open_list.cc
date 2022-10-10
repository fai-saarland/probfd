#include "open_list.h"

#include "../plugin.h"

namespace probfd {
namespace engine_interfaces {

bool OpenList<const ProbabilisticOperator*>::empty() const
{
    return size() == 0;
}

void OpenList<const ProbabilisticOperator*>::push(
    const StateID&,
    const ProbabilisticOperator*,
    const value_type::value_t&,
    const StateID& state_id)
{
    push(state_id);
}

void OpenList<const ProbabilisticOperator*>::set_state_id_map(
    StateIDMap<GlobalState>* state_id_map)
{
    this->state_id_map_ = state_id_map;
}

GlobalState
OpenList<const ProbabilisticOperator*>::lookup_state(const StateID& state_id)
{
    return this->state_id_map_->get_state(state_id);
}

} // namespace engine_interfaces

static PluginTypePlugin<GlobalStateOpenList>
    _plugin_type("GlobalStateOpenList", "");

} // namespace probfd
