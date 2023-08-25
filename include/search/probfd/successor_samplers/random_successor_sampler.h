#ifndef PROBFD_SUCCESSOR_SAMPLERS_RANDOM_SUCCESSOR_SAMPLER_H
#define PROBFD_SUCCESSOR_SAMPLERS_RANDOM_SUCCESSOR_SAMPLER_H

#include "probfd/algorithms/successor_sampler.h"

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
class RandomSuccessorSampler : public algorithms::SuccessorSampler<Action> {
    std::shared_ptr<utils::RandomNumberGenerator> rng_;

public:
    explicit RandomSuccessorSampler(const plugins::Options& opts);

    explicit RandomSuccessorSampler(
        std::shared_ptr<utils::RandomNumberGenerator> rng);

protected:
    StateID sample(
        StateID,
        Action,
        const Distribution<StateID>& successors,
        algorithms::StateProperties&) override;
};

} // namespace successor_samplers
} // namespace probfd

#include "probfd/successor_samplers/random_successor_sampler_impl.h"

#endif // __RANDOM_SUCCESSOR_SAMPLER_H__