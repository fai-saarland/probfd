#include "random_successor_sampler.h"

#include "../../option_parser.h"
#include "../../plugin.h"

#include <memory>

namespace probabilistic {
namespace successor_sampler {

GlobalState
RandomSuccessorSampler::sample(
    const GlobalState&,
    const ProbabilisticOperator*,
    const Distribution<GlobalState>& successors)
{
    return sampler_(successors);
}

static Plugin<TransitionSampler> _plugin(
    "random",
    options::parse_without_options<TransitionSampler, RandomSuccessorSampler>);

} // namespace successor_sampler
} // namespace probabilistic

