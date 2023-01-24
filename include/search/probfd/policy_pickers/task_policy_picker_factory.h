#ifndef PROBFD_POLICY_PICKER_TASK_POLICY_PICKER_FACTORY_H
#define PROBFD_POLICY_PICKER_TASK_POLICY_PICKER_FACTORY_H

#include <memory>

class State;
class OperatorID;

namespace probfd {

namespace engine_interfaces {
template <typename>
class StateIDMap;
template <typename>
class ActionIDMap;
template <typename>
class PolicyPicker;
} // namespace engine_interfaces

class TaskPolicyPickerFactory {
public:
    virtual ~TaskPolicyPickerFactory() = default;

    virtual std::shared_ptr<engine_interfaces::PolicyPicker<OperatorID>>
    create_policy_tiebreaker(
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<OperatorID>* op_id_map) = 0;
};

} // namespace probfd

#endif