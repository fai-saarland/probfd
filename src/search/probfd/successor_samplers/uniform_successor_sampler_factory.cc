#include "probfd/successor_samplers/uniform_successor_sampler_factory.h"
#include "probfd/successor_samplers/uniform_successor_sampler.h"

#include "downward/utils/rng_options.h"

namespace probfd {
namespace successor_samplers {

UniformSuccessorSamplerFactory::UniformSuccessorSamplerFactory(
    const plugins::Options& opts)
    : rng(utils::parse_rng_from_options(opts))
{
}

std::shared_ptr<TaskSuccessorSampler>
UniformSuccessorSamplerFactory::create_sampler(TaskStateSpace*)
{
    return std::make_shared<UniformSuccessorSampler>(rng);
}

} // namespace successor_samplers
} // namespace probfd
