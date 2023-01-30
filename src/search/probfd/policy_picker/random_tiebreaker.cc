#include "probfd/policy_picker/random_tiebreaker.h"

#include "utils/rng.h"

#include "operator_id.h"

namespace probfd {
namespace policy_tiebreaking {

RandomTiebreaker::RandomTiebreaker(
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : rng(rng)
{
}

int RandomTiebreaker::pick(
    const StateID&,
    const ActionID&,
    const std::vector<OperatorID>& options,
    const std::vector<Distribution<StateID>>&)
{
    return rng->random(options.size());
}

} // namespace policy_tiebreaking
} // namespace probfd
