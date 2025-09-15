#ifndef PROBFD_HEURISTICS_TASK_DEPENDENT_HEURISTIC_H
#define PROBFD_HEURISTICS_TASK_DEPENDENT_HEURISTIC_H

#include "probfd/fdr_types.h"
#include "probfd/heuristic.h"
#include "probfd/probabilistic_task.h"

#include "downward/utils/logging.h"

#include <memory>

/// This namespace contains heuristic implementations.
namespace probfd::heuristics {

class TaskDependentHeuristic : public FDRHeuristic {
protected:
    SharedProbabilisticTask task_;
    mutable downward::utils::LogProxy log_;

public:
    TaskDependentHeuristic(
        SharedProbabilisticTask task,
        downward::utils::LogProxy log);
};

} // namespace probfd::heuristics

#endif