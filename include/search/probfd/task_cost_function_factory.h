#ifndef PROBFD_TASK_COST_FUNCTION_FACTORY_H
#define PROBFD_TASK_COST_FUNCTION_FACTORY_H

#include "probfd/task_types.h"

#include <memory>

namespace probfd {

class ProbabilisticTask;

class TaskCostFunctionFactory {
public:
    virtual ~TaskCostFunctionFactory() = default;

    virtual std::unique_ptr<TaskCostFunction>
    create_cost_function(std::shared_ptr<ProbabilisticTask> task) = 0;
};

} // namespace probfd

#endif // HEURISTIC_FACTORY_H