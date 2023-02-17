#include "probfd/transition_samplers/arbitrary_selector.h"

namespace probfd {
namespace transition_samplers {

StateID ArbitrarySuccessorSelector::sample(
    StateID,
    const OperatorID&,
    const Distribution<StateID>& successors,
    engine_interfaces::HeuristicSearchInterface&)
{
    auto it = successors.begin();
    return it->item;
}

} // namespace transition_samplers
} // namespace probfd
