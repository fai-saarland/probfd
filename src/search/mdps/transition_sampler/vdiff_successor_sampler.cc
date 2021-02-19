#include "vdiff_successor_sampler.h"

#include "../../option_parser.h"
#include "../../plugin.h"
#include "../engines/heuristic_search_state_information.h"

namespace probabilistic {
namespace transition_sampler {

VDiffSuccessorSampler::VDiffSuccessorSampler(const options::Options& opts)
    : prefer_large_gaps_(opts.get<bool>("prefer_large_gaps"))
{
}

void
VDiffSuccessorSampler::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<bool>("prefer_large_gaps", "", "true");
}

StateID
VDiffSuccessorSampler::sample(
    const StateID&,
    const ProbabilisticOperator*,
    const Distribution<StateID>& successors)
{
    biased_.clear();
    value_type::value_t sum = 0;
    for (auto it = successors.begin(); it != successors.end(); ++it) {
        const value_type::value_t error =
            lookup_dual_bounds(it->first)->error_bound();
        const value_type::value_t p = it->second
            * (prefer_large_gaps_ ? error : (value_type::one - error));
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
    "value_gap_successor_sampler",
    options::
        parse<ProbabilisticOperatorTransitionSampler, VDiffSuccessorSampler>);

} // namespace transition_sampler
} // namespace probabilistic
