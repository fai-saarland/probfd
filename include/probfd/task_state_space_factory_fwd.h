#ifndef PROBFD_TASK_STATE_SPACE_FACTORY_FWD_H
#define PROBFD_TASK_STATE_SPACE_FACTORY_FWD_H

#include "probfd/task_dependent_factory_fwd.h"

namespace probfd {
class TaskStateSpace;
} // namespace probfd

namespace probfd {

using TaskStateSpaceFactory = TaskDependentFactory<TaskStateSpace>;

} // namespace probfd

#endif
