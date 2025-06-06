#ifndef PROBFD_TASKS_ROOT_TASK_H
#define PROBFD_TASKS_ROOT_TASK_H

#include <filesystem>
#include <memory>
#include <ostream>

namespace probfd {
class ProbabilisticTask;
}

/// This namespace contains implementations of probabilistic planning tasks.
namespace probfd::tasks {

/// Reads a probabilistic planning task in probabilistic SAS format from an
/// input stream.
extern std::unique_ptr<ProbabilisticTask> read_sas_task(std::istream& in);

/// Reads a probabilistic planning task in probabilistic SAS format from a file.
extern std::unique_ptr<ProbabilisticTask>
read_sas_task(const std::filesystem::path& filepath);

extern std::shared_ptr<ProbabilisticTask> read_root_tasks(std::istream& in);
extern std::shared_ptr<ProbabilisticTask>
read_root_tasks_from_file(const std::filesystem::path& filepath);

} // namespace probfd::tasks

#endif
