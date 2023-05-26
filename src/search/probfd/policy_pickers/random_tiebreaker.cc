#include "probfd/policy_pickers/random_tiebreaker.h"

#include "utils/rng.h"
#include "utils/rng_options.h"

#include "operator_id.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace policy_pickers {

RandomTiebreaker::RandomTiebreaker(const options::Options& opts)
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
    engine_interfaces::StateSpace<State, OperatorID>&,
    StateID,
    std::optional<OperatorID>,
    const std::vector<OperatorID>& options,
    const std::vector<Distribution<StateID>>&,
    engine_interfaces::HeuristicSearchInterface&)
{
    return rng->random(options.size());
}

void RandomTiebreaker::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<bool>("stable_policy", "", "true");
    utils::add_rng_options(parser);
}

} // namespace policy_pickers
} // namespace probfd
