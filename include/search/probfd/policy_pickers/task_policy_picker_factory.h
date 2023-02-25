#ifndef PROBFD_POLICY_PICKER_TASK_POLICY_PICKER_FACTORY_H
#define PROBFD_POLICY_PICKER_TASK_POLICY_PICKER_FACTORY_H

#include <memory>

class State;
class OperatorID;

namespace probfd {

namespace engine_interfaces {
template <typename, typename>
class StateSpace;
template <typename>
class PolicyPicker;
} // namespace engine_interfaces

/// Factory interface for policy pickers.
class TaskPolicyPickerFactory {
public:
    virtual ~TaskPolicyPickerFactory() = default;

    /// Creates a policy picker from a given state and action id map.
    virtual std::shared_ptr<engine_interfaces::PolicyPicker<OperatorID>>
    create_policy_tiebreaker(
        engine_interfaces::StateSpace<State, OperatorID>* state_space) = 0;
};

} // namespace probfd

#endif