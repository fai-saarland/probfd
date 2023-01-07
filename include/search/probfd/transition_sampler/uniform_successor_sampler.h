#ifndef PROBFD_TRANSITION_SAMPLER_UNIFORM_SUCCESSOR_SAMPLER_H
#define PROBFD_TRANSITION_SAMPLER_UNIFORM_SUCCESSOR_SAMPLER_H

#include "probfd/engine_interfaces/transition_sampler.h"

#include "probfd/utils/distribution_uniform_sampler.h"

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
namespace transition_sampler {

class UniformSuccessorSampler : public TaskTransitionSampler {
    distribution_uniform_sampler::DistributionUniformSampler sampler_;

public:
    explicit UniformSuccessorSampler(
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

#endif // __UNIFORM_SUCCESSOR_SAMPLER_H__