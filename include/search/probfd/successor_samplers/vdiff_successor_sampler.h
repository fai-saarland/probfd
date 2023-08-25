#ifndef PROBFD_SUCCESSOR_SAMPLERS_VDIFF_SUCCESSOR_SAMPLER_H
#define PROBFD_SUCCESSOR_SAMPLERS_VDIFF_SUCCESSOR_SAMPLER_H

#include "probfd/algorithms/successor_sampler.h"

#include "probfd/distribution.h"

#include <memory>

namespace plugins {
class Options;
}

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
namespace successor_samplers {

template <typename Action>
class VDiffSuccessorSampler : public algorithms::SuccessorSampler<Action> {
    const std::shared_ptr<utils::RandomNumberGenerator> rng_;
    const bool prefer_large_gaps_;

    Distribution<StateID> biased_;

public:
    explicit VDiffSuccessorSampler(const plugins::Options& opts);

    explicit VDiffSuccessorSampler(
        std::shared_ptr<utils::RandomNumberGenerator> rng,
        bool prefer_large_gaps);

protected:
    StateID sample(
        StateID state,
        Action action,
        const Distribution<StateID>& successors,
        algorithms::StateProperties& properties) override;
};

} // namespace successor_samplers
} // namespace probfd

#include "probfd/successor_samplers/vdiff_successor_sampler_impl.h"

#endif // __VDIFF_SUCCESSOR_SAMPLER_H__