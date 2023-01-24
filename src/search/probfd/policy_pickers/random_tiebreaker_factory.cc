#include "probfd/policy_pickers/random_tiebreaker_factory.h"
#include "probfd/policy_pickers/random_tiebreaker.h"

#include "utils/rng.h"
#include "utils/rng_options.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace policy_pickers {

RandomTiebreakerFactory::RandomTiebreakerFactory(const options::Options& opts)
    : rng(utils::parse_rng_from_options(opts))
{
}

void RandomTiebreakerFactory::add_options_to_parser(
    options::OptionParser& parser)
{
    utils::add_rng_options(parser);
}

std::shared_ptr<engine_interfaces::PolicyPicker<OperatorID>>
RandomTiebreakerFactory::create_policy_tiebreaker(
    engine_interfaces::StateIDMap<State>*,
    engine_interfaces::ActionIDMap<OperatorID>*)
{
    return std::make_shared<RandomTiebreaker>(rng);
}

} // namespace policy_pickers
} // namespace probfd
