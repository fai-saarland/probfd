#include "open_list.h"

#include "../plugin.h"

namespace probfd {

bool GlobalStateOpenList::empty() const
{
    return size() == 0;
}

void GlobalStateOpenList::push(
    const StateID&,
    const ProbabilisticOperator*,
    const value_type::value_t&,
    const StateID& state_id)
{
    push(state_id);
}

void GlobalStateOpenList::set_state_id_map(
    StateIDMap<GlobalState>* state_id_map)
{
    this->state_id_map_ = state_id_map;
}

GlobalState GlobalStateOpenList::lookup_state(const StateID& state_id)
{
    return this->state_id_map_->get_state(state_id);
}

static PluginTypePlugin<GlobalStateOpenList>
    _plugin_type("GlobalStateOpenList", "");

} // namespace probfd
