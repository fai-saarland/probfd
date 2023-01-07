#ifndef PROBFD_TRANSITION_SAMPLER_VDIFF_SUCCESSOR_SAMPLER_H
#define PROBFD_TRANSITION_SAMPLER_VDIFF_SUCCESSOR_SAMPLER_H

#include "probfd/engine_interfaces/transition_sampler.h"

#include "probfd/utils/distribution_random_sampler.h"

#include <memory>

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
namespace transition_sampler {

class VDiffSuccessorSampler : public TaskTransitionSampler {
    Distribution<StateID> biased_;
    distribution_random_sampler::DistributionRandomSampler sampler_;

    const bool prefer_large_gaps_;

public:
    explicit VDiffSuccessorSampler(
        std::shared_ptr<utils::RandomNumberGenerator> rng,
        bool prefer_large_gaps);

protected:
    virtual StateID sample(
        const StateID& state,
        const OperatorID& op,
        const Distribution<StateID>& successors,
        engine_interfaces::HeuristicSearchInterface& hs_interface) override;
};

} // namespace transition_sampler
} // namespace probfd

#endif // __VDIFF_SUCCESSOR_SAMPLER_H__