#include "probfd/successor_samplers/arbitrary_sampler.h"

#include "probfd/labelled_successor_pmf.h"
#include "probfd/pmf.h"

namespace probfd::successor_samplers {

template <typename Action>
StateID ArbitrarySuccessorSampler<Action>::sample(
    StateID,
    Action,
    const SuccessorPMF& successors,
    algorithms::StateProperties&)
{
    return successors.non_source_successor_pmf.begin()->item;
}

} // namespace probfd::successor_samplers
