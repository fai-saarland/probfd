#ifndef PROBFD_TRANSITION_SORTERS_TASK_SUCCESSOR_SORTER_FACTORY_H
#define PROBFD_TRANSITION_SORTERS_TASK_SUCCESSOR_SORTER_FACTORY_H

#include <memory>

class State;
class OperatorID;

namespace probfd {

namespace engine_interfaces {
template <typename, typename>
class StateSpace;
template <typename>
class TransitionSorter;
} // namespace engine_interfaces

/// Factory interface for successor sorters.
class TaskTransitionSorterFactory {
public:
    virtual ~TaskTransitionSorterFactory() = default;

    /// Creates a successor sorter from a given state and action id map.
    virtual std::shared_ptr<engine_interfaces::TransitionSorter<OperatorID>>
    create_transition_sorter(
        engine_interfaces::StateSpace<State, OperatorID>* state_space) = 0;
};

} // namespace probfd

#endif