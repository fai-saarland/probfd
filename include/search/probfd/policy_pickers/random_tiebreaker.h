#ifndef MDPS_POLICY_PICKER_RANDOM_TIEBREAKER_H
#define MDPS_POLICY_PICKER_RANDOM_TIEBREAKER_H

#include "probfd/engine_interfaces/policy_picker.h"

#include <memory>

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
namespace policy_pickers {

class RandomTiebreaker : public TaskPolicyPicker {
    std::shared_ptr<utils::RandomNumberGenerator> rng;

public:
    RandomTiebreaker(std::shared_ptr<utils::RandomNumberGenerator> rng);

protected:
    virtual int pick(
        StateID state,
        ActionID prev_policy,
        const std::vector<OperatorID>& action_choices,
        const std::vector<Distribution<StateID>>& successors,
        engine_interfaces::HeuristicSearchInterface& hs_interface) override;
};

} // namespace policy_pickers
} // namespace probfd

#endif // __RANDOM_TIEBREAKER_H__