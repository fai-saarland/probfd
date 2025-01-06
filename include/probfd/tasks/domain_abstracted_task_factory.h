#ifndef PROBFD_TASKS_DOMAIN_ABSTRACTED_TASK_FACTORY_H
#define PROBFD_TASKS_DOMAIN_ABSTRACTED_TASK_FACTORY_H

#include <memory>
#include <unordered_map>
#include <vector>

// Forward Declarations
namespace probfd {
class ProbabilisticTask;
}

namespace probfd::extra_tasks {

using ValueGroup = std::vector<int>;
using ValueGroups = std::vector<ValueGroup>;
using VarToGroups = std::unordered_map<int, ValueGroups>;

/*
  Factory for creating domain abstractions.
*/
std::shared_ptr<ProbabilisticTask> build_domain_abstracted_task(
    const std::shared_ptr<ProbabilisticTask>& parent,
    const VarToGroups& value_groups);

} // namespace probfd::extra_tasks

#endif
