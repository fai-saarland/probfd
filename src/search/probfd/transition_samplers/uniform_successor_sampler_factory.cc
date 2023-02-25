#include "probfd/transition_samplers/uniform_successor_sampler_factory.h"
#include "probfd/transition_samplers/uniform_successor_sampler.h"

#include "utils/rng_options.h"

namespace probfd {
namespace transition_samplers {

UniformSuccessorSamplerFactory::UniformSuccessorSamplerFactory(
    const options::Options& opts)
    : rng(utils::parse_rng_from_options(opts))
{
}

void UniformSuccessorSamplerFactory::add_options_to_parser(
    options::OptionParser& parser)
{
    utils::add_rng_options(parser);
}

std::shared_ptr<engine_interfaces::TransitionSampler<OperatorID>>
UniformSuccessorSamplerFactory::create_sampler(
    engine_interfaces::StateSpace<State, OperatorID>*)
{
    return std::make_shared<UniformSuccessorSampler>(rng);
}

} // namespace transition_samplers
} // namespace probfd
