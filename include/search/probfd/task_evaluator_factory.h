#ifndef PROBFD_HEURISTIC_FACTORY_H
#define PROBFD_HEURISTIC_FACTORY_H

#include "probfd/task_types.h"

#include <memory>

namespace probfd {

class ProbabilisticTask;

class TaskEvaluatorFactory {
public:
    virtual ~TaskEvaluatorFactory() = default;

    virtual std::unique_ptr<TaskEvaluator> create_evaluator(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<TaskCostFunction> task_cost_function) = 0;
};

} // namespace probfd

#endif // HEURISTIC_FACTORY_H