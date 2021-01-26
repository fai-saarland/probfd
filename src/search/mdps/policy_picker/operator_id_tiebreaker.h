#pragma once

#include "../policy_picker.h"

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {
namespace policy_tiebreaking {

class OperatorIdTiebreaker : public ProbabilisticOperatorPolicyPicker {
public:
    OperatorIdTiebreaker(const options::Options&);
    static void add_options_to_parser(options::OptionParser&);

protected:
    virtual int pick(
        const StateID& state,
        const ActionID& prev_policy,
        const std::vector<const ProbabilisticOperator*>& action_choices,
        const std::vector<Distribution<StateID>>& successors) override;

private:
    const int ascending_;
};

} // namespace policy_tiebreaking
} // namespace probabilistic
