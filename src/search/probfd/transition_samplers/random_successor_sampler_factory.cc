#include "probfd/transition_samplers/random_successor_sampler_factory.h"
#include "probfd/transition_samplers/random_successor_sampler.h"

#include "utils/rng_options.h"

namespace probfd {
namespace transition_samplers {

RandomSuccessorSamplerFactory::RandomSuccessorSamplerFactory(
    const options::Options& opts)
    : rng(utils::parse_rng_from_options(opts))
{
}

void RandomSuccessorSamplerFactory::add_options_to_parser(
    options::OptionParser& parser)
{
    utils::add_rng_options(parser);
}

std::shared_ptr<engine_interfaces::TransitionSampler<OperatorID>>
RandomSuccessorSamplerFactory::create_sampler(
    engine_interfaces::StateSpace<State, OperatorID>*)
{
    return std::make_shared<RandomSuccessorSampler<OperatorID>>(rng);
}

} // namespace transition_samplers
} // namespace probfd
