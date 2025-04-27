#ifndef PROBFD_TASKS_DOMAIN_ABSTRACTED_TASK_FACTORY_H
#define PROBFD_TASKS_DOMAIN_ABSTRACTED_TASK_FACTORY_H

#include "probfd/probabilistic_task.h"

#include <memory>
#include <unordered_map>
#include <vector>

// Forward Declarations
namespace probfd::extra_tasks {

using ValueGroup = std::vector<int>;
using ValueGroups = std::vector<ValueGroup>;
using VarToGroups = std::unordered_map<int, ValueGroups>;

/*
  Factory for creating domain abstractions.
*/
SharedProbabilisticTask build_domain_abstracted_task(
    const SharedProbabilisticTask& parent,
    const VarToGroups& value_groups);

} // namespace probfd::extra_tasks

#endif
