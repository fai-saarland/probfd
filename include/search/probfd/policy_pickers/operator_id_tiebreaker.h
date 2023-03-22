#ifndef PROBFD_POLICY_PICKER_OPERATOR_ID_TIEBREAKER_H
#define PROBFD_POLICY_PICKER_OPERATOR_ID_TIEBREAKER_H

#include "probfd/engine_interfaces/policy_picker.h"

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probfd {
namespace policy_pickers {

class OperatorIdTiebreaker : public TaskPolicyPicker {
    const int ascending_;

public:
    explicit OperatorIdTiebreaker(const options::Options&);
    explicit OperatorIdTiebreaker(int ascending);

    static void add_options_to_parser(options::OptionParser& p);

protected:
    virtual int pick(
        engine_interfaces::StateSpace<State, OperatorID>& state_space,
        StateID state,
        ActionID prev_policy,
        const std::vector<OperatorID>& action_choices,
        const std::vector<Distribution<StateID>>& successors,
        engine_interfaces::HeuristicSearchInterface& hs_interface) override;
};

} // namespace policy_pickers
} // namespace probfd

#endif // __OPERATOR_ID_TIEBREAKER_H__