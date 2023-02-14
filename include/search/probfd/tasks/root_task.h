#ifndef PROBFD_TASKS_ROOT_TASK_H
#define PROBFD_TASKS_ROOT_TASK_H

#include "probfd/probabilistic_task.h"

namespace probfd {

/// This namespace contains implementations of probabilistic planning tasks.
namespace tasks {

/// The input probabilistic planning task.
extern std::shared_ptr<ProbabilisticTask> g_root_task;

std::unique_ptr<ProbabilisticTask> read_sas_task(std::istream& in);

extern void read_root_tasks(std::istream& in);
} // namespace tasks
} // namespace probfd

#endif
