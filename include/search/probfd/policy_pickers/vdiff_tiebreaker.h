#ifndef PROBFD_POLICY_PICKER_VDIFF_TIEBREAKER_H
#define PROBFD_POLICY_PICKER_VDIFF_TIEBREAKER_H

#include "probfd/engine_interfaces/policy_picker.h"

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probfd {
namespace policy_pickers {

class VDiffTiebreaker : public TaskPolicyPicker {
    const value_t favor_large_gaps_;

public:
    explicit VDiffTiebreaker(const options::Options&);
    explicit VDiffTiebreaker(value_t favor_large_gaps_);

    virtual int pick(
        engine_interfaces::StateSpace<State, OperatorID>& state_space,
        StateID state,
        ActionID prev_policy,
        const std::vector<OperatorID>& action_choices,
        const std::vector<Distribution<StateID>>& successors,
        engine_interfaces::HeuristicSearchInterface& hs_interface) override;

    static void add_options_to_parser(options::OptionParser& parser);
};

} // namespace policy_pickers
} // namespace probfd

#endif // __VDIFF_TIEBREAKER_H__