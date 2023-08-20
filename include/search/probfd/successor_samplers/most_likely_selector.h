#ifndef PROBFD_SUCCESSOR_SAMPLERS_MOST_LIKELY_SELECTOR_H
#define PROBFD_SUCCESSOR_SAMPLERS_MOST_LIKELY_SELECTOR_H

#include "probfd/algorithms/fdr_types.h"
#include "probfd/algorithms/successor_sampler.h"

#include "downward/operator_id.h"

namespace probfd {
namespace successor_samplers {

class MostLikelySuccessorSelector : public FDRSuccessorSampler {
protected:
    StateID sample(
        StateID state,
        OperatorID op,
        const Distribution<StateID>& successors,
        algorithms::StateProperties& properties) override;
};

} // namespace successor_samplers
} // namespace probfd

#endif // __MOST_LIKELY_SELECTOR_H__