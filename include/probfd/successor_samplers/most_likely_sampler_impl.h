#include "probfd/successor_samplers/most_likely_sampler.h"

#include "probfd/labelled_successor_pmf.h"
#include "probfd/pmf.h"

namespace probfd::successor_samplers {

template <typename Action>
StateID MostLikelySuccessorSampler<Action>::sample(
    StateID state_id,
    Action,
    const SuccessorPMF& successors,
    algorithms::StateProperties&)
{
    if (successors.non_source_probability == 0_vt) return state_id;

    value_t max = -INFINITE_VALUE;

    StateID res;

    for (const auto& [item, probability] :
         successors.non_source_successor_pmf) {
        if (probability > max) {
            max = probability;
            res = item;
        }
    }

    return res;
}

} // namespace probfd::successor_samplers
