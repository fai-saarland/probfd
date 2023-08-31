#ifndef PROBFD_HEURISTIC_FACTORY_H
#define PROBFD_HEURISTIC_FACTORY_H

#include "probfd/fdr_types.h"

#include <memory>

namespace probfd {

class ProbabilisticTask;

class TaskEvaluatorFactory {
public:
    virtual ~TaskEvaluatorFactory() = default;

    virtual std::unique_ptr<FDREvaluator> create_evaluator(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function) = 0;
};

} // namespace probfd

#endif // HEURISTIC_FACTORY_H