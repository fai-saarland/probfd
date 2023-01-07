#include "probfd/transition_sampler/uniform_successor_sampler.h"

#include "utils/rng.h"
#include "utils/rng_options.h"

#include "operator_id.h"
#include "option_parser.h"
#include "plugin.h"

#include <memory>

namespace probfd {
namespace transition_sampler {

UniformSuccessorSampler::UniformSuccessorSampler(
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : sampler_(rng)
{
}

StateID UniformSuccessorSampler::sample(
    const StateID&,
    const OperatorID&,
    const Distribution<StateID>& successors,
    engine_interfaces::HeuristicSearchInterface&)
{
    return sampler_(successors);
}

} // namespace transition_sampler
} // namespace probfd
