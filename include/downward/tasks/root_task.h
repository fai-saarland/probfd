#ifndef TASKS_ROOT_TASK_H
#define TASKS_ROOT_TASK_H

#include "downward/abstract_task.h"

#include <iosfwd>
#include <memory>

namespace downward::tasks {
extern UniqueAbstractTask read_root_task(std::istream& in);
} // namespace tasks
#endif
