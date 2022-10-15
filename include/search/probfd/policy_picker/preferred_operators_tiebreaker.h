#ifndef MDPS_POLICY_PICKER_PREFERRED_OPERATORS_TIEBREAKER_H
#define MDPS_POLICY_PICKER_PREFERRED_OPERATORS_TIEBREAKER_H

#include "probfd/policy_picker.h"

#include <memory>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probfd {

namespace new_state_handlers {
class StorePreferredOperators;
}

namespace policy_tiebreaking {

class PreferredOperatorsTiebreaker : public ProbabilisticOperatorPolicyPicker {
public:
    explicit PreferredOperatorsTiebreaker(const options::Options&);
    static void add_options_to_parser(options::OptionParser&);

protected:
    virtual int pick(
        const StateID& state,
        const ActionID& prev_policy,
        const std::vector<const ProbabilisticOperator*>& action_choices,
        const std::vector<Distribution<StateID>>& successors) override;

    std::shared_ptr<new_state_handlers::StorePreferredOperators> pref_ops_;
};

} // namespace policy_tiebreaking
} // namespace probfd

#endif // __PREFERRED_OPERATORS_TIEBREAKER_H__