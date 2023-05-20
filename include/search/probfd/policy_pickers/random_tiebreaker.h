#ifndef PROBFD_POLICY_PICKER_RANDOM_TIEBREAKER_H
#define PROBFD_POLICY_PICKER_RANDOM_TIEBREAKER_H

#include "probfd/policy_pickers/stable_policy_picker.h"

#include <memory>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
namespace policy_pickers {

class RandomTiebreaker : public TaskStablePolicyPicker<RandomTiebreaker> {
    std::shared_ptr<utils::RandomNumberGenerator> rng;

public:
    explicit RandomTiebreaker(const options::Options&);
    explicit RandomTiebreaker(
        bool stable_policy,
        std::shared_ptr<utils::RandomNumberGenerator> rng);

    static void add_options_to_parser(options::OptionParser& parser);

    int pick_index(
        engine_interfaces::StateSpace<State, OperatorID>& state_space,
        StateID state,
        std::optional<OperatorID> prev_policy,
        const std::vector<OperatorID>& action_choices,
        const std::vector<Distribution<StateID>>& successors,
        engine_interfaces::HeuristicSearchInterface<OperatorID>& hs_interface);
};

} // namespace policy_pickers
} // namespace probfd

#endif // __RANDOM_TIEBREAKER_H__