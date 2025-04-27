#ifndef PROBFD_TASK_HEURISTIC_FACTORY_H
#define PROBFD_TASK_HEURISTIC_FACTORY_H

#include "probfd/fdr_types.h"
#include "probfd/probabilistic_task.h"

#include <memory>

namespace probfd {

class TaskHeuristicFactory {
public:
    virtual ~TaskHeuristicFactory() = default;

    virtual std::unique_ptr<FDREvaluator> create_heuristic(
        const SharedProbabilisticTask& task) = 0;
};

} // namespace probfd

#endif // PROBFD_TASK_HEURISTIC_FACTORY_H