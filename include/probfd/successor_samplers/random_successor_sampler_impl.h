#include "probfd/successor_samplers/random_successor_sampler.h"

#include "probfd/transition_tail.h"

#include "downward/utils/rng.h"

namespace probfd::successor_samplers {

template <typename Action>
RandomSuccessorSampler<Action>::RandomSuccessorSampler(int random_seed)
    : rng_(std::make_shared<utils::RandomNumberGenerator>(random_seed))
{
}

template <typename Action>
RandomSuccessorSampler<Action>::RandomSuccessorSampler(
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : rng_(std::move(rng))
{
}

template <typename Action>
StateID RandomSuccessorSampler<Action>::sample(
    StateID,
    Action,
    const SuccessorDistribution& successors,
    algorithms::StateProperties&)
{
    return successors.sample(*rng_)->item;
}

} // namespace probfd::successor_samplers
