#include "probfd/successor_samplers/random_successor_sampler_factory.h"
#include "probfd/successor_samplers/random_successor_sampler.h"

#include "utils/rng_options.h"

#include "operator_id.h"

namespace probfd {
namespace successor_samplers {

RandomSuccessorSamplerFactory::RandomSuccessorSamplerFactory(
    const plugins::Options& opts)
    : rng(utils::parse_rng_from_options(opts))
{
}

void RandomSuccessorSamplerFactory::add_options_to_feature(
    plugins::Feature& feature)
{
    utils::add_rng_options(feature);
}

std::shared_ptr<engine_interfaces::SuccessorSampler<OperatorID>>
RandomSuccessorSamplerFactory::create_sampler(
    engine_interfaces::StateSpace<State, OperatorID>*)
{
    return std::make_shared<RandomSuccessorSampler<OperatorID>>(rng);
}

} // namespace successor_samplers
} // namespace probfd
