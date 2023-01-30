#include "probfd/transition_sampler/arbitrary_selector.h"

namespace probfd {
namespace transition_sampler {

StateID ArbitrarySuccessorSelector::sample(
    const StateID&,
    const OperatorID&,
    const Distribution<StateID>& successors)
{
    auto it = successors.begin();
    return it->element;
}

} // namespace transition_sampler
} // namespace probfd
