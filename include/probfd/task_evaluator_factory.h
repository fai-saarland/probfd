#ifndef PROBFD_TASK_EVALUATOR_FACTORY_H
#define PROBFD_TASK_EVALUATOR_FACTORY_H

#include "probfd/fdr_types.h"

#include <memory>

namespace probfd {
class ProbabilisticTask;
}

namespace probfd {

class TaskEvaluatorFactory {
public:
    virtual ~TaskEvaluatorFactory() = default;

    virtual std::unique_ptr<FDREvaluator> create_evaluator(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function) = 0;
};

} // namespace probfd

#endif // PROBFD_TASK_EVALUATOR_FACTORY_H