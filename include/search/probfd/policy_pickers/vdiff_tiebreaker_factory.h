#ifndef PROBFD_POLICY_PICKER_VDIFF_TIEBREAKER_FACTORY_H
#define PROBFD_POLICY_PICKER_VDIFF_TIEBREAKER_FACTORY_H

#include "probfd/policy_pickers/task_policy_picker_factory.h"

#include "probfd/value_type.h"

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probfd {

/// This namespace contains implementations of greedy policy selectors.
namespace policy_pickers {

class VDiffTiebreakerFactory : public TaskPolicyPickerFactory {
    const value_t favor_large_gaps_;

public:
    explicit VDiffTiebreakerFactory(const options::Options&);
    ~VDiffTiebreakerFactory() override = default;

    static void add_options_to_parser(options::OptionParser&);

    std::shared_ptr<engine_interfaces::PolicyPicker<OperatorID>>
    create_policy_tiebreaker(
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<OperatorID>* op_id_map) override;
};

} // namespace policy_pickers
} // namespace probfd

#endif // __VDIFF_TIEBREAKER_H__