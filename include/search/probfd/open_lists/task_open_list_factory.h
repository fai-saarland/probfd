#ifndef PROBFD_OPEN_LISTS_TASK_OPEN_LIST_FACTORY_H
#define PROBFD_OPEN_LISTS_TASK_OPEN_LIST_FACTORY_H

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
class OpenList;
} // namespace engine_interfaces

class TaskOpenListFactory {
public:
    virtual ~TaskOpenListFactory() = default;

    virtual std::shared_ptr<engine_interfaces::OpenList<OperatorID>>
    create_open_list(
        engine_interfaces::HeuristicSearchConnector* connector,
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<OperatorID>* op_id_map) = 0;
};

} // namespace probfd

#endif