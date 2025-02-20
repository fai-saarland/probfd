#include "probfd/successor_samplers/arbitrary_sampler.h"

#include "probfd/distribution.h"
#include "probfd/transition_tail.h"

namespace probfd::successor_samplers {

template <typename Action>
StateID ArbitrarySuccessorSampler<Action>::sample(
    StateID,
    Action,
    const SuccessorDistribution& successors,
    algorithms::StateProperties&)
{
    return successors.non_source_successor_dist.begin()->item;
}

} // namespace probfd::successor_samplers
