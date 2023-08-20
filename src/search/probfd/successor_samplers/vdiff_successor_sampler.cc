#include "probfd/successor_samplers/vdiff_successor_sampler.h"

#include "probfd/engines/state_properties.h"

#include "downward/utils/rng.h"

namespace probfd {
namespace successor_samplers {

VDiffSuccessorSampler::VDiffSuccessorSampler(
    std::shared_ptr<utils::RandomNumberGenerator> rng,
    bool prefer_large_gaps)
    : rng_(rng)
    , prefer_large_gaps_(prefer_large_gaps)
{
}

StateID VDiffSuccessorSampler::sample(
    StateID,
    OperatorID,
    const Distribution<StateID>& successors,
    engines::StateProperties& properties)
{
    biased_.clear();

    value_t sum = 0;
    for (const auto& [item, probability] : successors) {
        const value_t error = properties.lookup_bounds(item).length();
        const value_t p =
            probability * (prefer_large_gaps_ ? error : (1_vt - error));
        if (p > 0_vt) {
            sum += p;
            biased_.add_probability(item, p);
        }
    }
    if (biased_.empty()) {
        return successors.sample(*rng_)->item;
    }
    biased_.normalize(1_vt / sum);
    return biased_.sample(*rng_)->item;
}

} // namespace successor_samplers
} // namespace probfd
