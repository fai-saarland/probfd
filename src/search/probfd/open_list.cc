#include "probfd/open_list.h"

#include "plugin.h"

namespace probfd {
namespace engine_interfaces {

bool OpenList<OperatorID>::empty() const
{
    return size() == 0;
}

void OpenList<OperatorID>::push(
    const StateID&,
    OperatorID,
    const value_type::value_t&,
    const StateID& state_id)
{
    push(state_id);
}

void OpenList<OperatorID>::set_state_id_map(StateIDMap<State>* state_id_map)
{
    this->state_id_map_ = state_id_map;
}

State OpenList<OperatorID>::lookup_state(const StateID& state_id)
{
    return this->state_id_map_->get_state(state_id);
}

} // namespace engine_interfaces

static PluginTypePlugin<GlobalStateOpenList>
    _plugin_type("GlobalStateOpenList", "");

} // namespace probfd
