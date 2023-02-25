#ifndef PROBFD_POLICY_PICKER_ARBITRARY_TIEBREAKER_FACTORY_H
#define PROBFD_POLICY_PICKER_ARBITRARY_TIEBREAKER_FACTORY_H

#include "probfd/policy_pickers/task_policy_picker_factory.h"

namespace probfd {
namespace policy_pickers {

class ArbitraryTiebreakerFactory : public TaskPolicyPickerFactory {
public:
    ~ArbitraryTiebreakerFactory() override = default;

    std::shared_ptr<engine_interfaces::PolicyPicker<OperatorID>>
    create_policy_tiebreaker(
        engine_interfaces::StateSpace<State, OperatorID>* state_space) override;
};

} // namespace policy_pickers
} // namespace probfd

#endif // __ARBITRARY_TIEBREAKER_H__