#ifndef PROBFD_POLICY_PICKER_RANDOM_TIEBREAKER_H
#define PROBFD_POLICY_PICKER_RANDOM_TIEBREAKER_H

#include "probfd/policy_pickers/stable_policy_picker.h"

#include "probfd/task_types.h"

#include <memory>

namespace plugins {
class Options;
class Feature;
} // namespace plugins

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
namespace policy_pickers {

class RandomTiebreaker : public TaskStablePolicyPicker<RandomTiebreaker> {
    std::shared_ptr<utils::RandomNumberGenerator> rng;

public:
    explicit RandomTiebreaker(const plugins::Options&);
    explicit RandomTiebreaker(
        bool stable_policy,
        std::shared_ptr<utils::RandomNumberGenerator> rng);

    int pick_index(
        TaskMDP& mdp,
        StateID state,
        std::optional<OperatorID> prev_policy,
        const std::vector<OperatorID>& action_choices,
        const std::vector<Distribution<StateID>>& successors,
        engine_interfaces::StateProperties& properties);
};

} // namespace policy_pickers
} // namespace probfd

#endif // __RANDOM_TIEBREAKER_H__