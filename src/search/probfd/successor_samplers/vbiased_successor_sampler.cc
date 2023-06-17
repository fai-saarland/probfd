#include "probfd/successor_samplers/vbiased_successor_sampler.h"

#include "probfd/engine_interfaces/state_properties.h"

#include "downward/utils/rng.h"

#include "downward/operator_id.h"

namespace probfd {
namespace successor_samplers {

VBiasedSuccessorSampler::VBiasedSuccessorSampler(
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : rng_(rng)
{
}

StateID VBiasedSuccessorSampler::sample(
    StateID,
    OperatorID,
    const Distribution<StateID>& successors,
    engine_interfaces::StateProperties& properties)
{
    biased_.clear();

    value_t sum = 0;
    for (const auto& [item, probability] : successors) {
        const auto p = probability * properties.lookup_value(item);
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
