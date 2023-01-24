#include "probfd/transition_samplers/most_likely_selector.h"

namespace probfd {
namespace transition_samplers {

StateID MostLikelySuccessorSelector::sample(
    const StateID&,
    const OperatorID&,
    const Distribution<StateID>& successors,
    engine_interfaces::HeuristicSearchInterface&)
{
    value_t max = 0_vt;
    StateID res = successors.begin()->element;
    for (auto it = successors.begin(); it != successors.end(); ++it) {
        if (it->probability > max) {
            max = it->probability;
            res = it->element;
        }
    }
    return res;
}

} // namespace transition_samplers
} // namespace probfd
