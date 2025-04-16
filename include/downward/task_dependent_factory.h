#ifndef TASK_DEPENDENT_FACTORY_H
#define TASK_DEPENDENT_FACTORY_H

#include "downward/task_dependent_factory_fwd.h"

#include <memory>

namespace downward {
class AbstractTask;
}

namespace downward {

template <typename T>
class TaskDependentFactory {
public:
    virtual ~TaskDependentFactory() = default;

    virtual std::unique_ptr<T>
    create_object(const std::shared_ptr<AbstractTask>& task) = 0;
};

}

#endif
