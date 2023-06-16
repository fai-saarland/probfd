#include "probfd/successor_samplers/uniform_successor_sampler_factory.h"
#include "probfd/successor_samplers/uniform_successor_sampler.h"

#include "utils/rng_options.h"

namespace probfd {
namespace successor_samplers {

UniformSuccessorSamplerFactory::UniformSuccessorSamplerFactory(
    const plugins::Options& opts)
    : rng(utils::parse_rng_from_options(opts))
{
}

std::shared_ptr<engine_interfaces::SuccessorSampler<OperatorID>>
UniformSuccessorSamplerFactory::create_sampler(
    engine_interfaces::StateSpace<State, OperatorID>*)
{
    return std::make_shared<UniformSuccessorSampler>(rng);
}

} // namespace successor_samplers
} // namespace probfd
