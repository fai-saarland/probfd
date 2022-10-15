#include "probfd/transition_sampler/most_likely_selector.h"

#include "option_parser.h"
#include "plugin.h"

#include <memory>

namespace probfd {
namespace transition_sampler {

StateID MostLikelySuccessorSelector::sample(
    const StateID&,
    const ProbabilisticOperator*,
    const Distribution<StateID>& successors)
{
    value_type::value_t max = value_type::zero;
    StateID res = successors.begin()->first;
    for (auto it = successors.begin(); it != successors.end(); ++it) {
        if (it->second > max) {
            max = it->second;
            res = it->first;
        }
    }
    return res;
}

static Plugin<ProbabilisticOperatorTransitionSampler> _plugin(
    "most_likely_successor_selector",
    options::parse_without_options<
        ProbabilisticOperatorTransitionSampler,
        MostLikelySuccessorSelector>);

} // namespace transition_sampler
} // namespace probfd
