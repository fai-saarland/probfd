#include "probfd/transition_sampler/vbiased_successor_sampler.h"

#include "utils/rng.h"
#include "utils/rng_options.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace transition_sampler {

VBiasedSuccessorSampler::VBiasedSuccessorSampler(const options::Options& opts)
    : sampler_(utils::parse_rng_from_options(opts))
{
}

VBiasedSuccessorSampler::VBiasedSuccessorSampler(
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : sampler_(rng)
{
}

void VBiasedSuccessorSampler::add_options_to_parser(
    options::OptionParser& parser)
{
    utils::add_rng_options(parser);
}

StateID VBiasedSuccessorSampler::sample(
    const StateID&,
    OperatorID,
    const Distribution<StateID>& successors)
{
    biased_.clear();
    value_type::value_t sum = 0;
    for (auto it = successors.begin(); it != successors.end(); ++it) {
        const auto p = it->probability * lookup_value(it->element);
        if (p > value_type::zero) {
            sum += p;
            biased_.add(it->element, p);
        }
    }
    if (biased_.empty()) {
        return sampler_(successors);
    }
    biased_.normalize(value_type::one / sum);
    return sampler_(biased_);
}

static Plugin<ProbabilisticOperatorTransitionSampler> _plugin(
    "vbiased_successor_sampler",
    options::
        parse<ProbabilisticOperatorTransitionSampler, VBiasedSuccessorSampler>);

} // namespace transition_sampler
} // namespace probfd
