#include "probfd/transition_samplers/vdiff_successor_sampler_factory.h"
#include "probfd/transition_samplers/vdiff_successor_sampler.h"

#include "utils/rng_options.h"

#include "option_parser.h"

namespace probfd {
namespace transition_samplers {

VDiffSuccessorSamplerFactory::VDiffSuccessorSamplerFactory(
    const options::Options& opts)
    : rng(utils::parse_rng_from_options(opts))
    , prefer_large_gaps(opts.get<bool>("prefer_large_gaps"))
{
}

void VDiffSuccessorSamplerFactory::add_options_to_parser(
    options::OptionParser& parser)
{
    utils::add_rng_options(parser);
    parser.add_option<bool>("prefer_large_gaps", "", "true");
}

std::shared_ptr<engine_interfaces::TransitionSampler<OperatorID>>
VDiffSuccessorSamplerFactory::create_sampler(
    engine_interfaces::StateSpace<State, OperatorID>*)
{
    return std::make_shared<VDiffSuccessorSampler>(rng, prefer_large_gaps);
}

} // namespace transition_samplers
} // namespace probfd
