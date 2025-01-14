#ifndef TASK_STATE_SPACE_FACTORY_H
#define TASK_STATE_SPACE_FACTORY_H

#include "probfd/fdr_types.h"

#include <memory>

namespace probfd {
class TaskStateSpace;
class ProbabilisticTask;
} // namespace probfd

namespace probfd {

class TaskStateSpaceFactory {
public:
    virtual ~TaskStateSpaceFactory() = default;

    virtual std::unique_ptr<TaskStateSpace> create_state_space(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function) = 0;
};

} // namespace probfd

#endif // TASK_STATE_SPACE_FACTORY_H
