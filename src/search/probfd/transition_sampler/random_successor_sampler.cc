#include "probfd/transition_sampler/random_successor_sampler.h"

#include "utils/rng.h"
#include "utils/rng_options.h"

#include "option_parser.h"
#include "plugin.h"

#include <memory>

namespace probfd {
namespace transition_sampler {

RandomSuccessorSampler::RandomSuccessorSampler(const options::Options& opts)
    : RandomSuccessorSampler(utils::parse_rng_from_options(opts))
{
}

RandomSuccessorSampler::RandomSuccessorSampler(
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : sampler_(rng)
{
}

StateID RandomSuccessorSampler::sample(
    const StateID&,
    OperatorID,
    const Distribution<StateID>& successors)
{
    return sampler_(successors);
}

void RandomSuccessorSampler::add_options_to_parser(
    options::OptionParser& parser)
{
    utils::add_rng_options(parser);
}

static Plugin<ProbabilisticOperatorTransitionSampler> _plugin(
    "random_successor_sampler",
    options::
        parse<ProbabilisticOperatorTransitionSampler, RandomSuccessorSampler>);

} // namespace transition_sampler
} // namespace probfd
