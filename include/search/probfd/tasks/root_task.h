#ifndef PROBFD_TASKS_ROOT_TASK_H
#define PROBFD_TASKS_ROOT_TASK_H

#include "probfd/probabilistic_task.h"

#include <memory>
#include <ostream>

namespace probfd {

class ProbabilisticTask;

/// This namespace contains implementations of probabilistic planning tasks.
namespace tasks {

/// The input probabilistic planning task.
extern std::shared_ptr<ProbabilisticTask> g_root_task;

extern std::unique_ptr<ProbabilisticTask> read_sas_task(std::istream& in);

extern std::shared_ptr<ProbabilisticTask> read_root_tasks(std::istream& in);

extern void set_root_task(std::shared_ptr<ProbabilisticTask> task);

} // namespace tasks
} // namespace probfd

#endif
