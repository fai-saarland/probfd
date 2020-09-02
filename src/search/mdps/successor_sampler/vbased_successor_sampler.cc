#include "vbased_successor_sampler.h"

#include "../../option_parser.h"
#include "../../plugin.h"
#include "../algorithms/heuristic_search_base.h"

namespace probabilistic {
namespace successor_sampler {

VBasedSuccessorSampler::VBasedSuccessorSampler(const options::Options&) { }

void
VBasedSuccessorSampler::add_options_to_parser(options::OptionParser&)
{
}

GlobalState
VBasedSuccessorSampler::sample(
    const GlobalState&,
    const ProbabilisticOperator*,
    const Distribution<GlobalState>& successors)
{
    biased_.clear();
    value_type::value_t sum = 0;
    for (auto it = successors.begin(); it != successors.end(); ++it) {
        const value_type::value_t p =
            it->second * hs_interface_->lookup_state_info(it->first)->value;
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

static Plugin<TransitionSampler> _plugin(
    "vbased_sampler",
    options::parse<TransitionSampler, VBasedSuccessorSampler>);

} // namespace successor_sampler
} // namespace probabilistic
