#ifndef PROBFD_HEURISTICS_TASK_DEPENDENT_HEURISTIC_H
#define PROBFD_HEURISTICS_TASK_DEPENDENT_HEURISTIC_H

#include "probfd/fdr_types.h"
#include "probfd/heuristic.h"
#include "probfd/probabilistic_task.h"

/// This namespace contains heuristic implementations.
namespace probfd::heuristics {

class TaskDependentHeuristic : public FDRHeuristic {
protected:
    SharedProbabilisticTask task_;

public:
    explicit TaskDependentHeuristic(SharedProbabilisticTask task);
};

} // namespace probfd::heuristics

#endif