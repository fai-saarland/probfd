#include "probfd/successor_samplers/vdiff_successor_sampler.h"

#include "probfd/algorithms/state_properties.h"

#include "probfd/interval.h"

#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include "downward/plugins/plugin.h"

namespace probfd::successor_samplers {

template <typename Action>
VDiffSuccessorSampler<Action>::VDiffSuccessorSampler(
    const plugins::Options& opts)
    : rng_(utils::parse_rng_from_options(opts))
    , prefer_large_gaps_(opts.get<bool>("prefer_large_gaps"))
{
}

template <typename Action>
VDiffSuccessorSampler<Action>::VDiffSuccessorSampler(
    std::shared_ptr<utils::RandomNumberGenerator> rng,
    bool prefer_large_gaps)
    : rng_(std::move(rng))
    , prefer_large_gaps_(prefer_large_gaps)
{
}

template <typename Action>
StateID VDiffSuccessorSampler<Action>::sample(
    StateID,
    Action,
    const Distribution<StateID>& successors,
    algorithms::StateProperties& properties)
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

} // namespace probfd::successor_samplers
