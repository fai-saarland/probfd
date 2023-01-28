#ifndef PROBFD_TRANSITION_SAMPLERS_TASK_TRANSITION_SAMPLER_FACTORY_H
#define PROBFD_TRANSITION_SAMPLERS_TASK_TRANSITION_SAMPLER_FACTORY_H

#include <memory>

class State;
class OperatorID;

namespace probfd {

namespace engine_interfaces {
class HeuristicSearchConnector;
template <typename>
class StateIDMap;
template <typename>
class ActionIDMap;
template <typename>
class TransitionSampler;
} // namespace engine_interfaces

/// Factory interface for transition samplers.
class TaskTransitionSamplerFactory {
public:
    virtual ~TaskTransitionSamplerFactory() = default;

    /// Creates a transition sampler from a given state and action id map.
    virtual std::shared_ptr<engine_interfaces::TransitionSampler<OperatorID>>
    create_sampler(
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<OperatorID>* op_id_map) = 0;
};

} // namespace probfd

#endif