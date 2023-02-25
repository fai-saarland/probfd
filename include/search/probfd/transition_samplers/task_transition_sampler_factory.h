#ifndef PROBFD_TRANSITION_SAMPLERS_TASK_TRANSITION_SAMPLER_FACTORY_H
#define PROBFD_TRANSITION_SAMPLERS_TASK_TRANSITION_SAMPLER_FACTORY_H

#include <memory>

class State;
class OperatorID;

namespace probfd {

namespace engine_interfaces {
class HeuristicSearchConnector;
template <typename, typename>
class StateSpace;
template <typename>
class TransitionSampler;
} // namespace engine_interfaces

/// Factory interface for transition samplers.
class TaskTransitionSamplerFactory {
public:
    virtual ~TaskTransitionSamplerFactory() = default;

    /// Creates a transition sampler from a given state and action id map.
    virtual std::shared_ptr<engine_interfaces::TransitionSampler<OperatorID>>
    create_sampler(engine_interfaces::StateSpace<State, OperatorID>*) = 0;
};

} // namespace probfd

#endif