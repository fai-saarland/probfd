#include "probfd/transition_samplers/vdiff_successor_sampler.h"

#include "probfd/engine_interfaces/heuristic_search_interface.h"

#include "utils/rng.h"
#include "utils/rng_options.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace transition_samplers {

VDiffSuccessorSampler::VDiffSuccessorSampler(
    std::shared_ptr<utils::RandomNumberGenerator> rng,
    bool prefer_large_gaps)
    : rng_(rng)
    , prefer_large_gaps_(prefer_large_gaps)
{
}

StateID VDiffSuccessorSampler::sample(
    const StateID&,
    const OperatorID&,
    const Distribution<StateID>& successors,
    engine_interfaces::HeuristicSearchInterface& hs_interface)
{
    biased_.clear();
    value_t sum = 0;
    for (auto it = successors.begin(); it != successors.end(); ++it) {
        const value_t error =
            hs_interface.lookup_dual_bounds(it->item).length();
        const value_t p =
            it->probability * (prefer_large_gaps_ ? error : (1_vt - error));
        if (p > 0_vt) {
            sum += p;
            biased_.add(it->item, p);
        }
    }
    if (biased_.empty()) {
        return successors.sample(*rng_)->item;
    }
    biased_.normalize(1_vt / sum);
    return biased_.sample(*rng_)->item;
}

} // namespace transition_samplers
} // namespace probfd
