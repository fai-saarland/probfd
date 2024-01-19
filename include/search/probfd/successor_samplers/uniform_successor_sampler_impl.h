#include "probfd/successor_samplers/uniform_successor_sampler.h"

#include "probfd/distribution.h"

#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include <memory>
#include <utility>

namespace probfd::successor_samplers {

template <typename Action>
UniformSuccessorSampler<Action>::UniformSuccessorSampler(
    const plugins::Options& opts)
    : rng_(utils::parse_rng_from_options(opts))
{
}

template <typename Action>
UniformSuccessorSampler<Action>::UniformSuccessorSampler(
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : rng_(std::move(rng))
{
}

template <typename Action>
StateID UniformSuccessorSampler<Action>::sample(
    StateID,
    Action,
    const Distribution<StateID>& successors,
    algorithms::StateProperties&)
{
    assert(!successors.empty());
    return successors.begin()[rng_->random(successors.size())].item;
}

} // namespace probfd::successor_samplers
