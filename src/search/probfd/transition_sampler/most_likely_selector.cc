#include "probfd/transition_sampler/most_likely_selector.h"

namespace probfd {
namespace transition_sampler {

StateID MostLikelySuccessorSelector::sample(
    const StateID&,
    const OperatorID&,
    const Distribution<StateID>& successors,
    engine_interfaces::HeuristicSearchInterface&)
{
    value_type::value_t max = value_type::zero;
    StateID res = successors.begin()->element;
    for (auto it = successors.begin(); it != successors.end(); ++it) {
        if (it->probability > max) {
            max = it->probability;
            res = it->element;
        }
    }
    return res;
}

} // namespace transition_sampler
} // namespace probfd
