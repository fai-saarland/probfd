#ifndef PROBFD_SUCCESSOR_SAMPLERS_VBIASED_SUCCESSOR_SAMPLER_H
#define PROBFD_SUCCESSOR_SAMPLERS_VBIASED_SUCCESSOR_SAMPLER_H

#include "probfd/algorithms/successor_sampler.h"

#include "probfd/distribution.h"

#include <memory>

// Forward Declarations
namespace utils {
class RandomNumberGenerator;
}

namespace probfd::successor_samplers {

template <typename Action>
class VBiasedSuccessorSampler : public algorithms::SuccessorSampler<Action> {
    Distribution<StateID> biased_;
    std::shared_ptr<utils::RandomNumberGenerator> rng_;

public:
    explicit VBiasedSuccessorSampler(int random_seed);

    explicit VBiasedSuccessorSampler(
        std::shared_ptr<utils::RandomNumberGenerator> rng);

protected:
    StateID sample(
        StateID state,
        Action action,
        const Distribution<StateID>& successors,
        algorithms::StateProperties& properties) override;
};

} // namespace probfd::successor_samplers

#include "probfd/successor_samplers/vbiased_successor_sampler_impl.h"

#endif // PROBFD_SUCCESSOR_SAMPLERS_VBIASED_SUCCESSOR_SAMPLER_H