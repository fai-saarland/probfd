#ifndef PROBFD_TASK_STATE_SPACE_FACTORY_H
#define PROBFD_TASK_STATE_SPACE_FACTORY_H

#include "probabilistic_task.h"
#include "probfd/fdr_types.h"

#include <memory>

namespace probfd {
class TaskStateSpace;
} // namespace probfd

namespace probfd {

class TaskStateSpaceFactory {
public:
    virtual ~TaskStateSpaceFactory() = default;

    virtual std::unique_ptr<TaskStateSpace>
    create_state_space(const SharedProbabilisticTask& task) = 0;
};

} // namespace probfd

#endif // PROBFD_TASK_STATE_SPACE_FACTORY_H
