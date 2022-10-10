#include "vbiased_successor_sampler.h"

#include "../../option_parser.h"
#include "../../plugin.h"

namespace probfd {
namespace transition_sampler {

VBiasedSuccessorSampler::VBiasedSuccessorSampler(const options::Options&)
{
}

void VBiasedSuccessorSampler::add_options_to_parser(options::OptionParser&)
{
}

StateID VBiasedSuccessorSampler::sample(
    const StateID&,
    const ProbabilisticOperator*,
    const Distribution<StateID>& successors)
{
    biased_.clear();
    value_type::value_t sum = 0;
    for (auto it = successors.begin(); it != successors.end(); ++it) {
        const value_type::value_t p = it->second * lookup_value(it->first);
        if (p > value_type::zero) {
            sum += p;
            biased_.add(it->first, p);
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
