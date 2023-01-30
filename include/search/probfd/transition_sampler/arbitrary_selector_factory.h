#ifndef PROBFD_TRANSITION_SAMPLER_ARBITRARY_SELECTOR_FACTORY_H
#define PROBFD_TRANSITION_SAMPLER_ARBITRARY_SELECTOR_FACTORY_H

#include "probfd/transition_sampler/task_transition_sampler_factory.h"

namespace probfd {

/// Namespace dedicated to transition sampling.
namespace transition_sampler {

class ArbitrarySuccessorSelectorFactory : public TaskTransitionSamplerFactory {
public:
    ~ArbitrarySuccessorSelectorFactory() override = default;

    std::shared_ptr<engine_interfaces::TransitionSampler<OperatorID>>
    create_sampler(
        engine_interfaces::HeuristicSearchConnector* connector,
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<OperatorID>* op_id_map) override;
};

} // namespace transition_sampler
} // namespace probfd

#endif // __ARBITRARY_SELECTOR_H__