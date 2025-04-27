#ifndef TASK_DEPENDENT_FACTORY_H
#define TASK_DEPENDENT_FACTORY_H

#include "downward/abstract_task.h"
#include "downward/task_dependent_factory_fwd.h"

#include <memory>

namespace downward {

template <typename T>
class TaskDependentFactory {
public:
    virtual ~TaskDependentFactory() = default;

    virtual std::unique_ptr<T>
    create_object(const SharedAbstractTask& task) = 0;
};

}

#endif
