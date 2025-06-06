#ifndef TASKS_ROOT_TASK_H
#define TASKS_ROOT_TASK_H

#include "downward/abstract_task.h"

namespace downward::tasks {
extern std::shared_ptr<AbstractTask> g_root_task;
extern void read_root_task(std::istream& in);
} // namespace tasks
#endif
