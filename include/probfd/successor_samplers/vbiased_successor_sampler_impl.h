#include "probfd/successor_samplers/vbiased_successor_sampler.h"

#include "probfd/algorithms/state_properties.h"

#include "probfd/successor_samplers/utils.h"

#include "downward/utils/rng.h"

namespace probfd::successor_samplers {

template <typename Action>
VBiasedSuccessorSampler<Action>::VBiasedSuccessorSampler(int random_seed)
    : rng_(
          std::make_shared<downward::utils::RandomNumberGenerator>(random_seed))
{
}

template <typename Action>
VBiasedSuccessorSampler<Action>::VBiasedSuccessorSampler(
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng)
    : rng_(std::move(rng))
{
}

template <typename Action>
StateID VBiasedSuccessorSampler<Action>::sample(
    StateID,
    Action,
    const SuccessorDistribution& successors,
    algorithms::StateProperties& properties)
{
    assert(!successors.non_source_successor_dist.empty());

    biased_.clear();

    value_t sum = 0;
    for (const auto& [item, probability] :
         successors.non_source_successor_dist) {
        const auto p = probability * properties.lookup_value(item);
        if (p > 0_vt) {
            sum += p;
            biased_.add_probability(item, p);
        }
    }

    if (biased_.empty()) { return successors.sample(*rng_)->item; }

    return weighted_select(biased_, sum, *rng_)->item;
}

} // namespace probfd::successor_samplers
