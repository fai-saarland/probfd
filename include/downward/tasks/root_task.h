#ifndef TASKS_ROOT_TASK_H
#define TASKS_ROOT_TASK_H

#include <iosfwd>
#include <memory>

namespace downward {
class AbstractTask;
}

namespace downward::tasks {
extern std::unique_ptr<AbstractTask> read_root_task(std::istream& in);
} // namespace tasks
#endif
