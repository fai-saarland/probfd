#ifndef PROBFD_SUCCESSOR_SAMPLERS_MOST_LIKELY_SELECTOR_H
#define PROBFD_SUCCESSOR_SAMPLERS_MOST_LIKELY_SELECTOR_H

#include "probfd/engine_interfaces/successor_sampler.h"
#include "probfd/engine_interfaces/task_types.h"

#include "downward/operator_id.h"

namespace probfd {
namespace successor_samplers {

class MostLikelySuccessorSelector : public TaskSuccessorSampler {
protected:
    StateID sample(
        StateID state,
        OperatorID op,
        const Distribution<StateID>& successors,
        engine_interfaces::StateProperties& properties) override;
};

} // namespace successor_samplers
} // namespace probfd

#endif // __MOST_LIKELY_SELECTOR_H__