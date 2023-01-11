#ifndef PROBFD_TASK_POLICY_H
#define PROBFD_TASK_POLICY_H

#include "operator_id.h"

#include "probfd/state_id_map.h
#include "probfd/types.h"

#include <unordered_map>

namespace probfd {

class Policy {
    std::unordered_map<StateID, ActionID> state_to_action_;

public:
    ActionID& operator[](StateID sid) { return state_to_action_[sid]; }

    const ActionID& operator[](StateID sid) const
    {
        return state_to_action_.find(sid)->second;
    }
};

} // namespace probfd

#endif