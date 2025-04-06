#ifndef PROBFD_SUCCESSOR_SAMPLERS_VDIFF_SUCCESSOR_SAMPLER_H
#define PROBFD_SUCCESSOR_SAMPLERS_VDIFF_SUCCESSOR_SAMPLER_H

#include "probfd/algorithms/successor_sampler.h"

#include "probfd/distribution.h"

#include <memory>

// Forward Declarations
namespace downward::utils {
class RandomNumberGenerator;
}

namespace probfd::successor_samplers {

template <typename Action>
class VDiffSuccessorSampler : public algorithms::SuccessorSampler<Action> {
    const std::shared_ptr<downward::utils::RandomNumberGenerator> rng_;
    const bool prefer_large_gaps_;

    Distribution<StateID> biased_;

public:
    explicit VDiffSuccessorSampler(int random_seed, bool prefer_large_gaps);

    explicit VDiffSuccessorSampler(
        std::shared_ptr<downward::utils::RandomNumberGenerator> rng,
        bool prefer_large_gaps);

protected:
    StateID sample(
        StateID state,
        Action action,
        const SuccessorDistribution& successors,
        algorithms::StateProperties& properties) override;
};

} // namespace probfd::successor_samplers

#include "probfd/successor_samplers/vdiff_successor_sampler_impl.h"

#endif // PROBFD_SUCCESSOR_SAMPLERS_VDIFF_SUCCESSOR_SAMPLER_H