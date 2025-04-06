#include "probfd/successor_samplers/uniform_successor_sampler.h"

#include "probfd/distribution.h"

#include "downward/utils/rng.h"

#include <memory>
#include <utility>

namespace probfd::successor_samplers {

template <typename Action>
UniformSuccessorSampler<Action>::UniformSuccessorSampler(int random_seed)
    : rng_(std::make_shared<downward::utils::RandomNumberGenerator>(random_seed))
{
}

template <typename Action>
UniformSuccessorSampler<Action>::UniformSuccessorSampler(
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng)
    : rng_(std::move(rng))
{
}

template <typename Action>
StateID UniformSuccessorSampler<Action>::sample(
    StateID,
    Action,
    const SuccessorDistribution& successors,
    algorithms::StateProperties&)
{
    assert(!successors.non_source_successor_dist.empty());
    const auto num_ns_successors = successors.non_source_successor_dist.size();
    const auto index = rng_->random(num_ns_successors);
    return successors.non_source_successor_dist.begin()[index].item;
}

} // namespace probfd::successor_samplers
