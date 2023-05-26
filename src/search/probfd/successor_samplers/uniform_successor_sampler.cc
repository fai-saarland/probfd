#include "probfd/successor_samplers/uniform_successor_sampler.h"

#include "utils/rng.h"
#include "utils/rng_options.h"

#include "operator_id.h"
#include "option_parser.h"
#include "plugin.h"

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
    engine_interfaces::StateProperties&)
{
    assert(!successors.empty());
    return successors.begin()[rng_->random(successors.size())].item;
}

} // namespace successor_samplers
} // namespace probfd
