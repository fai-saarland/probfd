#ifndef PROBFD_SUCCESSOR_SAMPLERS_RANDOM_SUCCESSOR_SAMPLER_H
#define PROBFD_SUCCESSOR_SAMPLERS_RANDOM_SUCCESSOR_SAMPLER_H

#include "probfd/engine_interfaces/successor_sampler.h"

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
namespace successor_samplers {

template <typename Action>
class RandomSuccessorSampler
    : public engine_interfaces::SuccessorSampler<Action> {
    std::shared_ptr<utils::RandomNumberGenerator> rng_;

public:
    explicit RandomSuccessorSampler(
        std::shared_ptr<utils::RandomNumberGenerator> rng)
        : rng_(rng)
    {
    }

protected:
    StateID sample(
        StateID,
        Action,
        const Distribution<StateID>& successors,
        engine_interfaces::StateProperties&) override
    {
        return successors.sample(*rng_)->item;
    }
};

} // namespace successor_samplers
} // namespace probfd

#endif // __RANDOM_SUCCESSOR_SAMPLER_H__