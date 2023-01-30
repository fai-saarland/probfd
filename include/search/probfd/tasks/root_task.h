#ifndef PROBFD_TASKS_ROOT_TASK_H
#define PROBFD_TASKS_ROOT_TASK_H

#include "probfd/probabilistic_task.h"

namespace probfd {
namespace tasks {
extern std::shared_ptr<ProbabilisticTask> g_root_task;
extern void read_root_tasks(std::istream& in);
} // namespace tasks
} // namespace probfd

#endif
