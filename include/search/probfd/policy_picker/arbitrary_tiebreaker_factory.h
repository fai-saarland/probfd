#ifndef MDPS_POLICY_PICKER_ARBITRARY_TIEBREAKER_FACTORY_H
#define MDPS_POLICY_PICKER_ARBITRARY_TIEBREAKER_FACTORY_H

#include "probfd/policy_picker/task_policy_picker_factory.h"

namespace probfd {
namespace policy_tiebreaking {

class ArbitraryTiebreakerFactory : public TaskPolicyPickerFactory {
public:
    ~ArbitraryTiebreakerFactory() override = default;

    std::shared_ptr<engine_interfaces::PolicyPicker<OperatorID>>
    create_policy_tiebreaker(
        engine_interfaces::HeuristicSearchConnector* connector,
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<OperatorID>* op_id_map) override;
};

} // namespace policy_tiebreaking
} // namespace probfd

#endif // __ARBITRARY_TIEBREAKER_H__