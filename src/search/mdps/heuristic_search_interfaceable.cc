#include "heuristic_search_interfaceable.h"

namespace probabilistic {

void
HeuristicSearchInterfaceable::initialize(
    HeuristicSearchConnector* connector,
    StateIDMap<GlobalState>* state_id_map,
    ActionIDMap<const ProbabilisticOperator*>* op_id_map)
{
    connector_ = connector;
    state_id_map_ = state_id_map;
    op_id_map_ = op_id_map;
    initialize();
}

GlobalState
HeuristicSearchInterfaceable::lookup_state(const StateID& state_id) const
{
    return state_id_map_->get_state(state_id);
}

const ProbabilisticOperator*
HeuristicSearchInterfaceable::lookup_operator(
    const StateID& state_id,
    const ActionID& action_id) const
{
    return op_id_map_->get_action(state_id, action_id);
}

} // namespace probabilistic

