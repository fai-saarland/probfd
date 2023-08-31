#include "probfd/successor_samplers/random_successor_sampler.h"

#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

namespace probfd {
namespace successor_samplers {

template <typename Action>
RandomSuccessorSampler<Action>::RandomSuccessorSampler(
    const plugins::Options& opts)
    : rng_(utils::parse_rng_from_options(opts))
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
    const Distribution<StateID>& successors,
    algorithms::StateProperties&)
{
    return successors.sample(*rng_)->item;
}

} // namespace successor_samplers
} // namespace probfd
