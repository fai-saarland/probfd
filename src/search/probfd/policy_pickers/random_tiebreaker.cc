#include "probfd/policy_pickers/random_tiebreaker.h"

#include "utils/rng.h"
#include "utils/rng_options.h"

#include "operator_id.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace policy_pickers {

RandomTiebreaker::RandomTiebreaker(const options::Options& opts)
    : rng(utils::parse_rng_from_options(opts))
{
}

RandomTiebreaker::RandomTiebreaker(
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : rng(rng)
{
}

int RandomTiebreaker::pick(
    engine_interfaces::StateSpace<State, OperatorID>&,
    StateID,
    ActionID,
    const std::vector<OperatorID>& options,
    const std::vector<Distribution<StateID>>&,
    engine_interfaces::HeuristicSearchInterface&)
{
    return rng->random(options.size());
}

void RandomTiebreaker::add_options_to_parser(options::OptionParser& parser)
{
    utils::add_rng_options(parser);
}

} // namespace policy_pickers
} // namespace probfd
