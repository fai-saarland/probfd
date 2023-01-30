#ifndef MDPS_POLICY_PICKER_VDIFF_TIEBREAKER_FACTORY_H
#define MDPS_POLICY_PICKER_VDIFF_TIEBREAKER_FACTORY_H

#include "probfd/policy_picker/task_policy_picker_factory.h"

#include "probfd/value_type.h"

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probfd {

/// Namespace dedicated to policy tiebreaker implementations.
namespace policy_tiebreaking {

class VDiffTiebreakerFactory : public TaskPolicyPickerFactory {
    const value_type::value_t favor_large_gaps_;

public:
    explicit VDiffTiebreakerFactory(const options::Options&);
    ~VDiffTiebreakerFactory() override = default;

    static void add_options_to_parser(options::OptionParser&);

    std::shared_ptr<engine_interfaces::PolicyPicker<OperatorID>>
    create_policy_tiebreaker(
        engine_interfaces::HeuristicSearchConnector* connector,
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<OperatorID>* op_id_map) override;
};

} // namespace policy_tiebreaking
} // namespace probfd

#endif // __VDIFF_TIEBREAKER_H__