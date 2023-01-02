#include "probfd/transition_sampler/uniform_successor_sampler.h"

#include "utils/rng.h"
#include "utils/rng_options.h"

#include "option_parser.h"
#include "plugin.h"

#include <memory>

namespace probfd {
namespace transition_sampler {

UniformSuccessorSampler::UniformSuccessorSampler(const options::Options& opts)
    : UniformSuccessorSampler(utils::parse_rng_from_options(opts))
{
}

UniformSuccessorSampler::UniformSuccessorSampler(
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : sampler_(rng)
{
}

StateID UniformSuccessorSampler::sample(
    const StateID&,
    OperatorID,
    const Distribution<StateID>& successors)
{
    return sampler_(successors);
}

void UniformSuccessorSampler::add_options_to_parser(
    options::OptionParser& parser)
{
    utils::add_rng_options(parser);
}

static Plugin<ProbabilisticOperatorTransitionSampler> _plugin(
    "uniform_random_successor_sampler",
    options::
        parse<ProbabilisticOperatorTransitionSampler, UniformSuccessorSampler>);

} // namespace transition_sampler
} // namespace probfd
