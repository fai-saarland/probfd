#include "probfd/transition_samplers/uniform_successor_sampler.h"

#include "utils/rng.h"
#include "utils/rng_options.h"

#include "operator_id.h"
#include "option_parser.h"
#include "plugin.h"

#include <memory>

namespace probfd {
namespace transition_samplers {

UniformSuccessorSampler::UniformSuccessorSampler(
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : rng_(rng)
{
}

StateID UniformSuccessorSampler::sample(
    const StateID&,
    const OperatorID&,
    const Distribution<StateID>& successors,
    engine_interfaces::HeuristicSearchInterface&)
{
    assert(!successors.empty());
    return successors.begin()[rng_->random(successors.size())].item;
}

} // namespace transition_samplers
} // namespace probfd
