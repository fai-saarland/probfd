#include "probfd/heuristic_search_interfaceable.h"

namespace probfd {

void HeuristicSearchInterfaceable::initialize(
    engine_interfaces::HeuristicSearchConnector* connector,
    engine_interfaces::StateIDMap<legacy::GlobalState>* state_id_map,
    engine_interfaces::ActionIDMap<const ProbabilisticOperator*>* op_id_map)
{
    connector_ = connector;
    state_id_map_ = state_id_map;
    op_id_map_ = op_id_map;
    initialize();
}

legacy::GlobalState
HeuristicSearchInterfaceable::lookup_state(const StateID& state_id) const
{
    return state_id_map_->get_state(state_id);
}

const ProbabilisticOperator* HeuristicSearchInterfaceable::lookup_operator(
    const StateID& state_id,
    const ActionID& action_id) const
{
    return op_id_map_->get_action(state_id, action_id);
}

} // namespace probfd
