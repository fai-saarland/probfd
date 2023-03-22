#ifndef PROBFD_TRANSITION_SAMPLERS_RANDOM_SUCCESSOR_SAMPLER_H
#define PROBFD_TRANSITION_SAMPLERS_RANDOM_SUCCESSOR_SAMPLER_H

#include "probfd/engine_interfaces/transition_sampler.h"

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
namespace transition_samplers {

template <typename Action>
class RandomSuccessorSampler
    : public engine_interfaces::TransitionSampler<Action> {
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
        param_type<Action>,
        const Distribution<StateID>& successors,
        engine_interfaces::HeuristicSearchInterface&) override
    {
        return successors.sample(*rng_)->item;
    }
};

} // namespace transition_samplers
} // namespace probfd

#endif // __RANDOM_SUCCESSOR_SAMPLER_H__