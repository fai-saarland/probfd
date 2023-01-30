#ifndef MDPS_ACTION_ID_MAP_H
#define MDPS_ACTION_ID_MAP_H

#include "probfd/engine_interfaces/action_id_map.h"

#include "operator_id.h"

#include <cassert>

namespace probfd {
namespace engine_interfaces {

template <>
struct ActionIDMap<OperatorID> {
    ActionID get_action_id(const StateID&, const OperatorID& op_id);
    OperatorID get_action(const StateID&, const ActionID& action_id);
};

inline ActionID
ActionIDMap<OperatorID>::get_action_id(const StateID&, const OperatorID& op_id)
{
    return ActionID(op_id.get_index());
}

inline OperatorID
ActionIDMap<OperatorID>::get_action(const StateID&, const ActionID& action_id)
{
    return OperatorID(action_id);
}

} // namespace engine_interfaces
} // namespace probfd

#endif // __ACTION_ID_MAP_H__