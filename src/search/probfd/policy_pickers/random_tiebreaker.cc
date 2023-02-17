#include "probfd/policy_pickers/random_tiebreaker.h"

#include "utils/rng.h"

#include "operator_id.h"

namespace probfd {
namespace policy_pickers {

RandomTiebreaker::RandomTiebreaker(
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : rng(rng)
{
}

int RandomTiebreaker::pick(
    StateID,
    const ActionID&,
    const std::vector<OperatorID>& options,
    const std::vector<Distribution<StateID>>&,
    engine_interfaces::HeuristicSearchInterface&)
{
    return rng->random(options.size());
}

} // namespace policy_pickers
} // namespace probfd
