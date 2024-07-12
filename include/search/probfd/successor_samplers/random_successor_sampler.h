#ifndef PROBFD_SUCCESSOR_SAMPLERS_RANDOM_SUCCESSOR_SAMPLER_H
#define PROBFD_SUCCESSOR_SAMPLERS_RANDOM_SUCCESSOR_SAMPLER_H

#include "probfd/algorithms/successor_sampler.h"

#include <memory>

// Forward Declarations
namespace utils {
class RandomNumberGenerator;
}

namespace probfd::successor_samplers {

template <typename Action>
class RandomSuccessorSampler : public algorithms::SuccessorSampler<Action> {
    std::shared_ptr<utils::RandomNumberGenerator> rng_;

public:
    explicit RandomSuccessorSampler(int random_seed);

    explicit RandomSuccessorSampler(
        std::shared_ptr<utils::RandomNumberGenerator> rng);

protected:
    StateID sample(
        StateID,
        Action,
        const Distribution<StateID>& successors,
        algorithms::StateProperties&) override;
};

} // namespace probfd::successor_samplers

#include "probfd/successor_samplers/random_successor_sampler_impl.h"

#endif // PROBFD_SUCCESSOR_SAMPLERS_RANDOM_SUCCESSOR_SAMPLER_H