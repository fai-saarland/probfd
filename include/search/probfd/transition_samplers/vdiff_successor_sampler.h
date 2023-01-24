#ifndef PROBFD_TRANSITION_SAMPLERS_VDIFF_SUCCESSOR_SAMPLER_H
#define PROBFD_TRANSITION_SAMPLERS_VDIFF_SUCCESSOR_SAMPLER_H

#include "probfd/engine_interfaces/transition_sampler.h"

#include <memory>

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
namespace transition_samplers {

class VDiffSuccessorSampler : public TaskTransitionSampler {
    Distribution<StateID> biased_;
    std::shared_ptr<utils::RandomNumberGenerator> rng_;

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

} // namespace transition_samplers
} // namespace probfd

#endif // __VDIFF_SUCCESSOR_SAMPLER_H__