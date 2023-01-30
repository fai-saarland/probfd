#include "probfd/policy_picker/random_tiebreaker.h"

#include "utils/rng.h"
#include "utils/rng_options.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace policy_tiebreaking {

RandomTiebreaker::RandomTiebreaker(const options::Options& opts)
    : rng(utils::parse_rng_from_options(opts))
{
}

void RandomTiebreaker::add_options_to_parser(options::OptionParser& parser)
{
    utils::add_rng_options(parser);
}

int RandomTiebreaker::pick(
    const StateID&,
    const ActionID&,
    const std::vector<OperatorID>& options,
    const std::vector<Distribution<StateID>>&)
{
    return rng->random(options.size());
}

static Plugin<ProbabilisticOperatorPolicyPicker> _plugin(
    "random_policy_tiebreaker",
    options::parse<ProbabilisticOperatorPolicyPicker, RandomTiebreaker>);

} // namespace policy_tiebreaking
} // namespace probfd
