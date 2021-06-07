#pragma once

#include "engine_interfaces/heuristic_search_connector.h"
#include "engine_interfaces/policy_picker.h"

#include "state_id_map.h"
#include "action_id_map.h"

#include "probabilistic_operator.h"

#include "../global_state.h"

class GlobalState;

namespace probabilistic {

class HeuristicSearchInterfaceable {
public:
    virtual ~HeuristicSearchInterfaceable() = default;

    void initialize(
        HeuristicSearchConnector* connector,
        StateIDMap<GlobalState>* state_id_map,
        ActionIDMap<const ProbabilisticOperator*>* op_id_map);

    virtual void print_statistics(std::ostream&) const { }

protected:
    virtual void initialize() { }

    GlobalState lookup_state(const StateID& state_id) const;

    const ProbabilisticOperator*
    lookup_operator(const StateID& state_id, const ActionID& action_id) const;

    const engines::heuristic_search::StateFlags*
    lookup_state_flags(const StateID& state_id) const
    {
        return reinterpret_cast<const engines::heuristic_search::StateFlags*>(
            connector_->lookup(state_id));
    }

    const value_type::value_t& lookup_value(const StateID& state_id) const
    {
        return reinterpret_cast<const value_utils::SingleValue*>(
            connector_->lookup(state_id))->value;
    }

    const value_utils::IntervalValue*
    lookup_dual_bounds(const StateID& state_id) const
    {
        return reinterpret_cast<const value_utils::IntervalValue*>(
            connector_->lookup(state_id));
    }

    ActionID lookup_policy(const StateID& state_id) const
    {
        return reinterpret_cast<
            const engines::heuristic_search::StatesPolicy<std::true_type>*>(
            connector_->lookup(state_id))->get_policy();
    }

private:
    HeuristicSearchConnector* connector_ = nullptr;
    StateIDMap<GlobalState>* state_id_map_ = nullptr;
    ActionIDMap<const ProbabilisticOperator*>* op_id_map_ = nullptr;
};

} // namespace probabilistic

