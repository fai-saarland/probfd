#include "most_likely_selector.h"

#include "../../option_parser.h"
#include "../../plugin.h"

#include <memory>

namespace probabilistic {
namespace successor_sampler {

GlobalState
MostLikelySuccessorSelector::sample(
    const GlobalState&,
    const ProbabilisticOperator*,
    const Distribution<GlobalState>& successors)
{
    value_type::value_t max = value_type::zero;
    GlobalState res = successors.begin()->first;
    for (auto it = successors.begin(); it != successors.end(); ++it) {
        if (it->second > max) {
            max = it->second;
            res = it->first;
        }
    }
    return res;
}

static Plugin<TransitionSampler> _plugin(
    "most_likely_successor",
    options::
        parse_without_options<TransitionSampler, MostLikelySuccessorSelector>);

} // namespace successor_sampler
} // namespace probabilistic

