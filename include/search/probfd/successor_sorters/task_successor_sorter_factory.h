#ifndef PROBFD_SUCCESSOR_SORTERS_TASK_SUCCESSOR_SORTER_FACTORY_H
#define PROBFD_SUCCESSOR_SORTERS_TASK_SUCCESSOR_SORTER_FACTORY_H

#include <memory>

class State;
class OperatorID;

namespace probfd {

namespace engine_interfaces {
template <typename>
class StateIDMap;
template <typename>
class ActionIDMap;
template <typename>
class SuccessorSorter;
} // namespace engine_interfaces

/// Factory interface for successor sorters.
class TaskSuccessorSorterFactory {
public:
    virtual ~TaskSuccessorSorterFactory() = default;

    /// Creates a successor sorter from a given state and action id map.
    virtual std::shared_ptr<engine_interfaces::SuccessorSorter<OperatorID>>
    create_successor_sorter(
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<OperatorID>* op_id_map) = 0;
};

} // namespace probfd

#endif