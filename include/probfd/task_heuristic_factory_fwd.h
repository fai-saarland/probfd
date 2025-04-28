#ifndef PROBFD_TASK_HEURISTIC_FACTORY_FWD_H
#define PROBFD_TASK_HEURISTIC_FACTORY_FWD_H

#include "probfd/task_dependent_factory_fwd.h"

#include "probfd/fdr_types.h"

namespace probfd {

using TaskHeuristicFactory = TaskDependentFactory<FDRHeuristic>;

} // namespace probfd

#endif