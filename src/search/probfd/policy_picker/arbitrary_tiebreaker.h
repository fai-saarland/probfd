#ifndef MDPS_POLICY_PICKER_ARBITRARY_TIEBREAKER_H
#define MDPS_POLICY_PICKER_ARBITRARY_TIEBREAKER_H

#include "../policy_picker.h"

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {
namespace policy_tiebreaking {

class ArbitraryTiebreaker : public ProbabilisticOperatorPolicyPicker {
public:
    explicit ArbitraryTiebreaker();
    explicit ArbitraryTiebreaker(const options::Options&);
    static void add_options_to_parser(options::OptionParser&);

protected:
    virtual int pick(
        const StateID& state,
        const ActionID& prev_policy,
        const std::vector<const ProbabilisticOperator*>& action_choices,
        const std::vector<Distribution<StateID>>& successors) override;
};

} // namespace policy_tiebreaking
} // namespace probabilistic

#endif // __ARBITRARY_TIEBREAKER_H__