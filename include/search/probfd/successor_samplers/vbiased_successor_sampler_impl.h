#include "probfd/successor_samplers/vbiased_successor_sampler.h"

#include "probfd/algorithms/state_properties.h"

#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

namespace probfd::successor_samplers {

template <typename Action>
VBiasedSuccessorSampler<Action>::VBiasedSuccessorSampler(
    const plugins::Options& opts)
    : rng_(utils::parse_rng_from_options(opts))
{
}

template <typename Action>
VBiasedSuccessorSampler<Action>::VBiasedSuccessorSampler(
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : rng_(std::move(rng))
{
}

template <typename Action>
StateID VBiasedSuccessorSampler<Action>::sample(
    StateID,
    Action,
    const Distribution<StateID>& successors,
    algorithms::StateProperties& properties)
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

} // namespace probfd::successor_samplers
