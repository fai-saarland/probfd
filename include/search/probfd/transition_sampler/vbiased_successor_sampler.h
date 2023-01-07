#ifndef PROBFD_TRANSITION_SAMPLER_VBIASED_SUCCESSOR_SAMPLER_H
#define PROBFD_TRANSITION_SAMPLER_VBIASED_SUCCESSOR_SAMPLER_H

#include "probfd/engine_interfaces/transition_sampler.h"

#include "probfd/utils/distribution_random_sampler.h"

#include <memory>

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {

namespace transition_sampler {

class VBiasedSuccessorSampler : public TaskTransitionSampler {
    Distribution<StateID> biased_;
    distribution_random_sampler::DistributionRandomSampler sampler_;

public:
    explicit VBiasedSuccessorSampler(
        std::shared_ptr<utils::RandomNumberGenerator> rng);

protected:
    virtual StateID sample(
        const StateID& state,
        const OperatorID& op,
        const Distribution<StateID>& successors,
        engine_interfaces::HeuristicSearchInterface& hs_interface) override;
};

} // namespace transition_sampler
} // namespace probfd

#endif // __VBIASED_SUCCESSOR_SAMPLER_H__