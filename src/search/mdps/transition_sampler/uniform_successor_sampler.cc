#include "uniform_successor_sampler.h"

#include "../../option_parser.h"
#include "../../plugin.h"

#include <memory>

namespace probabilistic {
namespace transition_sampler {

StateID UniformSuccessorSampler::sample(
    const StateID&,
    const ProbabilisticOperator*,
    const Distribution<StateID>& successors)
{
    return sampler_(successors);
}

static Plugin<ProbabilisticOperatorTransitionSampler> _plugin(
    "uniform_random_successor_sampler",
    options::parse_without_options<
        ProbabilisticOperatorTransitionSampler,
        UniformSuccessorSampler>);

} // namespace transition_sampler
} // namespace probabilistic
