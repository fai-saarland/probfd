#ifndef DOWNWARD_FROM_FILE_MUTEX_FACTORY_H
#define DOWNWARD_FROM_FILE_MUTEX_FACTORY_H

#include "downward/task_dependent_factory.h"

#include "downward/mutex_information.h"

#include <memory>
#include <set>
#include <string>
#include <vector>

namespace downward {
class AbstractTask;
struct FactPair;
class MutexInformation;
class VariablesProxy;
}

namespace downward {

class FromFileMutexFactory : public TaskDependentFactory<MutexInformation> {
    std::string filename;

public:
    explicit FromFileMutexFactory(std::string filename);

    std::unique_ptr<MutexInformation>
    create_object(const std::shared_ptr<AbstractTask>& task) override;
};

}

#endif
