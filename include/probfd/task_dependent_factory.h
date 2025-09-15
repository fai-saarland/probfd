#ifndef PROBFD_TASK_DEPENDENT_FACTORY_H
#define PROBFD_TASK_DEPENDENT_FACTORY_H

#include "probfd/task_dependent_factory_fwd.h"

#include "probfd/probabilistic_task.h"

#include <memory>

namespace probfd {

template <typename T>
class TaskDependentFactory {
public:
    virtual ~TaskDependentFactory() = default;

    virtual std::unique_ptr<T>
    create_object(const SharedProbabilisticTask& task) = 0;
};

}

#endif
