#ifndef MDPS_POLICY_PICKER_RANDOM_TIEBREAKER_H
#define MDPS_POLICY_PICKER_RANDOM_TIEBREAKER_H

#include "probfd/policy_picker.h"

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
namespace policy_tiebreaking {

class RandomTiebreaker : public ProbabilisticOperatorPolicyPicker {
    std::shared_ptr<utils::RandomNumberGenerator> rng;

public:
    explicit RandomTiebreaker(const options::Options& opts);
    
    static void add_options_to_parser(options::OptionParser&);

protected:
    virtual int pick(
        const StateID& state,
        const ActionID& prev_policy,
        const std::vector<OperatorID>& action_choices,
        const std::vector<Distribution<StateID>>& successors) override;
};

} // namespace policy_tiebreaking
} // namespace probfd

#endif // __RANDOM_TIEBREAKER_H__