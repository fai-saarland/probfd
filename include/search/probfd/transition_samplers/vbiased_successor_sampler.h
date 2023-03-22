#ifndef PROBFD_TRANSITION_SAMPLERS_VBIASED_SUCCESSOR_SAMPLER_H
#define PROBFD_TRANSITION_SAMPLERS_VBIASED_SUCCESSOR_SAMPLER_H

#include "probfd/engine_interfaces/transition_sampler.h"

#include "operator_id.h"

#include <memory>

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {

namespace transition_samplers {

class VBiasedSuccessorSampler : public TaskTransitionSampler {
    Distribution<StateID> biased_;
    std::shared_ptr<utils::RandomNumberGenerator> rng_;

public:
    explicit VBiasedSuccessorSampler(
        std::shared_ptr<utils::RandomNumberGenerator> rng);

protected:
    StateID sample(
        StateID state,
        OperatorID op,
        const Distribution<StateID>& successors,
        engine_interfaces::HeuristicSearchInterface& hs_interface) override;
};

} // namespace transition_samplers
} // namespace probfd

#endif // __VBIASED_SUCCESSOR_SAMPLER_H__