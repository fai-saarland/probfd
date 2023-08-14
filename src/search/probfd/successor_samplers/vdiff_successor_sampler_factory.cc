#include "probfd/successor_samplers/vdiff_successor_sampler_factory.h"
#include "probfd/successor_samplers/vdiff_successor_sampler.h"

#include "downward/utils/rng_options.h"

#include "downward/plugins/options.h"

namespace probfd {
namespace successor_samplers {

VDiffSuccessorSamplerFactory::VDiffSuccessorSamplerFactory(
    const plugins::Options& opts)
    : rng(utils::parse_rng_from_options(opts))
    , prefer_large_gaps(opts.get<bool>("prefer_large_gaps"))
{
}

std::shared_ptr<FDRSuccessorSampler>
VDiffSuccessorSamplerFactory::create_sampler(FDRMDP*)
{
    return std::make_shared<VDiffSuccessorSampler>(rng, prefer_large_gaps);
}

} // namespace successor_samplers
} // namespace probfd
