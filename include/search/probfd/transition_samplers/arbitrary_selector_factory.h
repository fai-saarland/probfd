#ifndef PROBFD_TRANSITION_SAMPLERS_ARBITRARY_SELECTOR_FACTORY_H
#define PROBFD_TRANSITION_SAMPLERS_ARBITRARY_SELECTOR_FACTORY_H

#include "probfd/transition_samplers/task_transition_sampler_factory.h"

namespace probfd {

/// Namespace dedicated to transition sampling.
namespace transition_samplers {

class ArbitrarySuccessorSelectorFactory : public TaskTransitionSamplerFactory {
public:
    ~ArbitrarySuccessorSelectorFactory() override = default;

    std::shared_ptr<engine_interfaces::TransitionSampler<OperatorID>>
    create_sampler(engine_interfaces::StateSpace<State, OperatorID>*) override;
};

} // namespace transition_sampler
} // namespace probfd

#endif // __ARBITRARY_SELECTOR_H__