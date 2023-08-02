#include "probfd/successor_samplers/vbiased_successor_sampler_factory.h"
#include "probfd/successor_samplers/vbiased_successor_sampler.h"

#include "downward/utils/rng_options.h"

namespace probfd {
namespace successor_samplers {

VBiasedSuccessorSamplerFactory::VBiasedSuccessorSamplerFactory(
    const plugins::Options& opts)
    : rng(utils::parse_rng_from_options(opts))
{
}

std::shared_ptr<TaskSuccessorSampler>
VBiasedSuccessorSamplerFactory::create_sampler(TaskMDP*)
{
    return std::make_shared<VBiasedSuccessorSampler>(rng);
}

} // namespace successor_samplers
} // namespace probfd
