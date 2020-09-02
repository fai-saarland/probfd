#include "uniform_successor_sampler.h"

#include "../../option_parser.h"
#include "../../plugin.h"

#include <memory>

namespace probabilistic {
namespace successor_sampler {

GlobalState
UniformSuccessorSampler::sample(
    const GlobalState&,
    const ProbabilisticOperator*,
    const Distribution<GlobalState>& successors)
{
    return sampler_(successors);
}

static Plugin<TransitionSampler> _plugin(
    "uniform_random",
    options::parse_without_options<TransitionSampler, UniformSuccessorSampler>);

} // namespace successor_sampler
} // namespace probabilistic

