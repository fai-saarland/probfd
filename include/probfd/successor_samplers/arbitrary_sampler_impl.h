#include "probfd/successor_samplers/arbitrary_sampler.h"

#include "probfd/distribution.h"

namespace probfd::successor_samplers {

template <typename Action>
StateID ArbitrarySuccessorSampler<Action>::sample(
    StateID,
    Action,
    const Distribution<StateID>& successors,
    algorithms::StateProperties&)
{
    return successors.begin()->item;
}

} // namespace probfd::successor_samplers
