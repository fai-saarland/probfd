#ifndef PROBFD_OPEN_LISTS_TASK_OPEN_LIST_FACTORY_H
#define PROBFD_OPEN_LISTS_TASK_OPEN_LIST_FACTORY_H

#include "probfd/engine_interfaces/types.h"

#include <memory>

namespace probfd {

/// Factory interface for open lists.
class TaskOpenListFactory {
public:
    virtual ~TaskOpenListFactory() = default;

    /// Creates a open list from a given state and action id map.
    virtual std::shared_ptr<TaskOpenList>
    create_open_list(TaskMDP* state_space) = 0;
};

} // namespace probfd

#endif