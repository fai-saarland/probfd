#include "probfd/transition_sampler/vdiff_successor_sampler.h"

#include "probfd/engines/heuristic_search_state_information.h"

#include "utils/rng.h"
#include "utils/rng_options.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace transition_sampler {

VDiffSuccessorSampler::VDiffSuccessorSampler(
    std::shared_ptr<utils::RandomNumberGenerator> rng,
    bool prefer_large_gaps)
    : sampler_(rng)
    , prefer_large_gaps_(prefer_large_gaps)
{
}

StateID VDiffSuccessorSampler::sample(
    const StateID&,
    const OperatorID&,
    const Distribution<StateID>& successors)
{
    biased_.clear();
    value_type::value_t sum = 0;
    for (auto it = successors.begin(); it != successors.end(); ++it) {
        const value_type::value_t error =
            connector_->lookup_dual_bounds(it->element)->error_bound();
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

} // namespace transition_sampler
} // namespace probfd
