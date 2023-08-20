#ifndef PROBFD_SUCCESSOR_SAMPLERS_UNIFORM_SUCCESSOR_SAMPLER_H
#define PROBFD_SUCCESSOR_SAMPLERS_UNIFORM_SUCCESSOR_SAMPLER_H

#include "probfd/algorithms/fdr_types.h"
#include "probfd/algorithms/successor_sampler.h"

#include "downward/operator_id.h"

#include <memory>

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
namespace successor_samplers {

class UniformSuccessorSampler : public FDRSuccessorSampler {
    std::shared_ptr<utils::RandomNumberGenerator> rng_;

public:
    explicit UniformSuccessorSampler(
        std::shared_ptr<utils::RandomNumberGenerator> rng);

protected:
    StateID sample(
        StateID state,
        OperatorID op,
        const Distribution<StateID>& successors,
        algorithms::StateProperties& properties) override final;
};

} // namespace successor_samplers
} // namespace probfd

#endif // __UNIFORM_SUCCESSOR_SAMPLER_H__