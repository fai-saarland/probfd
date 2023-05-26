#include "probfd/successor_samplers/arbitrary_selector.h"

namespace probfd {
namespace successor_samplers {

StateID ArbitrarySuccessorSelector::sample(
    StateID,
    OperatorID,
    const Distribution<StateID>& successors,
    engine_interfaces::HeuristicSearchInterface&)
{
    auto it = successors.begin();
    return it->item;
}

} // namespace successor_samplers
} // namespace probfd
