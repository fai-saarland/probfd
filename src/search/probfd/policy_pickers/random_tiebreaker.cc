#include "probfd/policy_pickers/random_tiebreaker.h"

#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include "downward/operator_id.h"

#include "downward/plugins/options.h"

namespace probfd {
namespace policy_pickers {

RandomTiebreaker::RandomTiebreaker(const plugins::Options& opts)
    : RandomTiebreaker(
          opts.get<bool>("stable_policy"),
          utils::parse_rng_from_options(opts))
{
}

RandomTiebreaker::RandomTiebreaker(
    bool stable_policy,
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : TaskStablePolicyPicker<RandomTiebreaker>(stable_policy)
    , rng(rng)
{
}

int RandomTiebreaker::pick_index(
    TaskMDP&,
    StateID,
    std::optional<OperatorID>,
    const std::vector<OperatorID>& options,
    const std::vector<Distribution<StateID>>&,
    engine_interfaces::StateProperties&)
{
    return rng->random(options.size());
}

} // namespace policy_pickers
} // namespace probfd
