#ifndef PROBFD_SUCCESSOR_SAMPLERS_TASK_SUCCESSOR_SAMPLER_FACTORY_H
#define PROBFD_SUCCESSOR_SAMPLERS_TASK_SUCCESSOR_SAMPLER_FACTORY_H

#include <memory>

class State;
class OperatorID;

namespace probfd {

namespace engine_interfaces {
class HeuristicSearchConnector;
template <typename, typename>
class StateSpace;
template <typename>
class SuccessorSampler;
} // namespace engine_interfaces

/// Factory interface for transition samplers.
class TaskSuccessorSamplerFactory {
public:
    virtual ~TaskSuccessorSamplerFactory() = default;

    /// Creates a transition sampler from a given state and action id map.
    virtual std::shared_ptr<engine_interfaces::SuccessorSampler<OperatorID>>
    create_sampler(engine_interfaces::StateSpace<State, OperatorID>*) = 0;
};

} // namespace probfd

#endif