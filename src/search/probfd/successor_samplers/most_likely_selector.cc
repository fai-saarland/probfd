#include "probfd/successor_samplers/most_likely_selector.h"

#include "probfd/distribution.h"

namespace probfd {
namespace successor_samplers {

StateID MostLikelySuccessorSelector::sample(
    StateID,
    OperatorID,
    const Distribution<StateID>& successors,
    algorithms::StateProperties&)
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

} // namespace successor_samplers
} // namespace probfd
