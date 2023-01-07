#ifndef PROBFD_TRANSITION_SAMPLER_RANDOM_SUCCESSOR_SAMPLER_H
#define PROBFD_TRANSITION_SAMPLER_RANDOM_SUCCESSOR_SAMPLER_H

#include "probfd/engine_interfaces/transition_sampler.h"

#include "probfd/utils/distribution_random_sampler.h"

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
namespace transition_sampler {

template <typename Action>
class RandomSuccessorSampler
    : public engine_interfaces::TransitionSampler<Action> {
    distribution_random_sampler::DistributionRandomSampler sampler_;

public:
    explicit RandomSuccessorSampler(
        std::shared_ptr<utils::RandomNumberGenerator> rng)
        : sampler_(rng)
    {
    }

protected:
    StateID sample(
        const StateID&,
        const Action&,
        const Distribution<StateID>& successors,
        engine_interfaces::HeuristicSearchInterface&) override
    {
        return sampler_(successors);
    }
};

} // namespace transition_sampler
} // namespace probfd

#endif // __RANDOM_SUCCESSOR_SAMPLER_H__