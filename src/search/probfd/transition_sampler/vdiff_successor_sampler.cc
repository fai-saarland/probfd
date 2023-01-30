#include "probfd/transition_sampler/vdiff_successor_sampler.h"

#include "probfd/engines/heuristic_search_state_information.h"

#include "utils/rng.h"
#include "utils/rng_options.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace transition_sampler {

VDiffSuccessorSampler::VDiffSuccessorSampler(const options::Options& opts)
    : sampler_(utils::parse_rng_from_options(opts))
    , prefer_large_gaps_(opts.get<bool>("prefer_large_gaps"))
{
}

VDiffSuccessorSampler::VDiffSuccessorSampler(
    std::shared_ptr<utils::RandomNumberGenerator> rng,
    bool prefer_large_gaps)
    : sampler_(rng)
    , prefer_large_gaps_(prefer_large_gaps)
{
}

void VDiffSuccessorSampler::add_options_to_parser(options::OptionParser& parser)
{
    utils::add_rng_options(parser);
    parser.add_option<bool>("prefer_large_gaps", "", "true");
}

StateID VDiffSuccessorSampler::sample(
    const StateID&,
    OperatorID,
    const Distribution<StateID>& successors)
{
    biased_.clear();
    value_type::value_t sum = 0;
    for (auto it = successors.begin(); it != successors.end(); ++it) {
        const value_type::value_t error =
            lookup_dual_bounds(it->element)->error_bound();
        const value_type::value_t p =
            it->probability *
            (prefer_large_gaps_ ? error : (value_type::one - error));
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
    "value_gap_successor_sampler",
    options::
        parse<ProbabilisticOperatorTransitionSampler, VDiffSuccessorSampler>);

} // namespace transition_sampler
} // namespace probfd
