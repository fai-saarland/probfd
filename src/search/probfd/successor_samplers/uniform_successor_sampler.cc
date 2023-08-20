#include "probfd/successor_samplers/uniform_successor_sampler.h"

#include "probfd/distribution.h"

#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include "downward/operator_id.h"

#include <memory>

namespace probfd {
namespace successor_samplers {

UniformSuccessorSampler::UniformSuccessorSampler(
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : rng_(rng)
{
}

StateID UniformSuccessorSampler::sample(
    StateID,
    OperatorID,
    const Distribution<StateID>& successors,
    engines::StateProperties&)
{
    assert(!successors.empty());
    return successors.begin()[rng_->random(successors.size())].item;
}

} // namespace successor_samplers
} // namespace probfd
