#include "probfd/transition_samplers/vbiased_successor_sampler_factory.h"
#include "probfd/transition_samplers/vbiased_successor_sampler.h"

#include "utils/rng.h"
#include "utils/rng_options.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace transition_samplers {

VBiasedSuccessorSamplerFactory::VBiasedSuccessorSamplerFactory(
    const options::Options& opts)
    : rng(utils::parse_rng_from_options(opts))
{
}

void VBiasedSuccessorSamplerFactory::add_options_to_parser(
    options::OptionParser& parser)
{
    utils::add_rng_options(parser);
}

std::shared_ptr<engine_interfaces::TransitionSampler<OperatorID>>
VBiasedSuccessorSamplerFactory::create_sampler(
    engine_interfaces::StateIDMap<State>*,
    engine_interfaces::ActionIDMap<OperatorID>*)
{
    return std::make_shared<VBiasedSuccessorSampler>(rng);
}

} // namespace transition_samplers
} // namespace probfd
