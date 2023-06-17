#ifndef PROBFD_SUCCESSOR_SAMPLERS_VDIFF_SUCCESSOR_SAMPLER_H
#define PROBFD_SUCCESSOR_SAMPLERS_VDIFF_SUCCESSOR_SAMPLER_H

#include "probfd/engine_interfaces/successor_sampler.h"

#include "probfd/distribution.h"

#include "downward/operator_id.h"

#include <memory>

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
namespace successor_samplers {

class VDiffSuccessorSampler : public TaskSuccessorSampler {
    Distribution<StateID> biased_;
    std::shared_ptr<utils::RandomNumberGenerator> rng_;

    const bool prefer_large_gaps_;

public:
    explicit VDiffSuccessorSampler(
        std::shared_ptr<utils::RandomNumberGenerator> rng,
        bool prefer_large_gaps);

protected:
    StateID sample(
        StateID state,
        OperatorID op,
        const Distribution<StateID>& successors,
        engine_interfaces::StateProperties& properties) override;
};

} // namespace successor_samplers
} // namespace probfd

#endif // __VDIFF_SUCCESSOR_SAMPLER_H__