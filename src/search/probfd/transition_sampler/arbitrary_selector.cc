#include "probfd/transition_sampler/arbitrary_selector.h"

#include "option_parser.h"
#include "plugin.h"

#include <memory>

namespace probfd {
namespace transition_sampler {

StateID ArbitrarySuccessorSelector::sample(
    const StateID&,
    const ProbabilisticOperator*,
    const Distribution<StateID>& successors)
{
    auto it = successors.begin();
    return it->first;
}

static Plugin<ProbabilisticOperatorTransitionSampler> _plugin(
    "arbitrary_successor_selector",
    options::parse_without_options<
        ProbabilisticOperatorTransitionSampler,
        ArbitrarySuccessorSelector>);

} // namespace transition_sampler
} // namespace probfd
