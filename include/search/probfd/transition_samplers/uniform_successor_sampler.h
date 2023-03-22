#ifndef PROBFD_TRANSITION_SAMPLERS_UNIFORM_SUCCESSOR_SAMPLER_H
#define PROBFD_TRANSITION_SAMPLERS_UNIFORM_SUCCESSOR_SAMPLER_H

#include "probfd/engine_interfaces/transition_sampler.h"

#include "operator_id.h"

#include <memory>

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
namespace transition_samplers {

class UniformSuccessorSampler : public TaskTransitionSampler {
    std::shared_ptr<utils::RandomNumberGenerator> rng_;

public:
    explicit UniformSuccessorSampler(
        std::shared_ptr<utils::RandomNumberGenerator> rng);

protected:
    StateID sample(
        StateID state,
        OperatorID op,
        const Distribution<StateID>& successors,
        engine_interfaces::HeuristicSearchInterface& hs_interface)
        override final;
};

} // namespace transition_samplers
} // namespace probfd

#endif // __UNIFORM_SUCCESSOR_SAMPLER_H__