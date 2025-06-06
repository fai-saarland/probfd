#ifndef PROBFD_SUCCESSOR_SAMPLERS_UNIFORM_SUCCESSOR_SAMPLER_H
#define PROBFD_SUCCESSOR_SAMPLERS_UNIFORM_SUCCESSOR_SAMPLER_H

#include "probfd/algorithms/successor_sampler.h"

#include <memory>

// Forward Declarations
namespace downward::utils {
class RandomNumberGenerator;
}

namespace probfd::successor_samplers {

template <typename Action>
class UniformSuccessorSampler : public algorithms::SuccessorSampler<Action> {
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng_;

public:
    explicit UniformSuccessorSampler(int random_seed);

    explicit UniformSuccessorSampler(
        std::shared_ptr<downward::utils::RandomNumberGenerator> rng);

protected:
    StateID sample(
        StateID state,
        Action action,
        const SuccessorDistribution& successors,
        algorithms::StateProperties& properties) final;
};

} // namespace probfd::successor_samplers

#include "probfd/successor_samplers/uniform_successor_sampler_impl.h"

#endif // PROBFD_SUCCESSOR_SAMPLERS_UNIFORM_SUCCESSOR_SAMPLER_H