#ifndef PROBFD_TRANSITION_SORTERS_TASK_SUCCESSOR_SORTER_FACTORY_H
#define PROBFD_TRANSITION_SORTERS_TASK_SUCCESSOR_SORTER_FACTORY_H

#include "probfd/engine_interfaces/task_types.h"

#include "probfd/task_types.h"

#include <memory>

namespace probfd {

/// Factory interface for successor sorters.
class TaskTransitionSorterFactory {
public:
    virtual ~TaskTransitionSorterFactory() = default;

    /// Creates a successor sorter from a given state and action id map.
    virtual std::shared_ptr<TaskTransitionSorter>
    create_transition_sorter(TaskMDP* state_space) = 0;
};

} // namespace probfd

#endif