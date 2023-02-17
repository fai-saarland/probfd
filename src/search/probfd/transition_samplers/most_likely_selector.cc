#include "probfd/transition_samplers/most_likely_selector.h"

namespace probfd {
namespace transition_samplers {

StateID MostLikelySuccessorSelector::sample(
    StateID,
    const OperatorID&,
    const Distribution<StateID>& successors,
    engine_interfaces::HeuristicSearchInterface&)
{
    value_t max = -INFINITE_VALUE;

    StateID res;

    for (const auto& [item, probability] : successors) {
        if (probability > max) {
            max = probability;
            res = item;
        }
    }

    return res;
}

} // namespace transition_samplers
} // namespace probfd
