#ifndef PROBFD_TRANSITION_SAMPLERS_MOST_LIKELY_SELECTOR_FACTORY_H
#define PROBFD_TRANSITION_SAMPLERS_MOST_LIKELY_SELECTOR_FACTORY_H

#include "probfd/transition_samplers/task_transition_sampler_factory.h"

namespace probfd {
namespace transition_samplers {

class MostLikelySuccessorSelectorFactory : public TaskTransitionSamplerFactory {
public:
    ~MostLikelySuccessorSelectorFactory() override = default;

    std::shared_ptr<engine_interfaces::TransitionSampler<OperatorID>>
    create_sampler(engine_interfaces::StateSpace<State, OperatorID>*) override;
};

} // namespace transition_samplers
} // namespace probfd

#endif // __MOST_LIKELY_SELECTOR_H__