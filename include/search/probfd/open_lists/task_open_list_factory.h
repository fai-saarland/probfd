#ifndef PROBFD_OPEN_LISTS_TASK_OPEN_LIST_FACTORY_H
#define PROBFD_OPEN_LISTS_TASK_OPEN_LIST_FACTORY_H

#include <memory>

class State;
class OperatorID;

namespace probfd {

namespace engine_interfaces {
template <typename, typename>
class StateSpace;
template <typename>
class OpenList;
} // namespace engine_interfaces

/// Factory interface for open lists.
class TaskOpenListFactory {
public:
    virtual ~TaskOpenListFactory() = default;

    /// Creates a open list from a given state and action id map.
    virtual std::shared_ptr<engine_interfaces::OpenList<OperatorID>>
    create_open_list(
        engine_interfaces::StateSpace<State, OperatorID>* state_space) = 0;
};

} // namespace probfd

#endif