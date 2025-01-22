#ifndef DOWNWARD_FROM_FILE_MUTEX_FACTORY_H
#define DOWNWARD_FROM_FILE_MUTEX_FACTORY_H

#include "downward/mutexes.h"

#include <memory>
#include <set>
#include <string>
#include <vector>

namespace downward {
class AbstractTask;
struct FactPair;
class VariablesProxy;
}

namespace downward {

class FromFileMutexFactory : public MutexFactory {
    std::string filename;

public:
    explicit FromFileMutexFactory(std::string filename);

    std::shared_ptr<MutexInformation>
    compute_mutexes(const std::shared_ptr<AbstractTask>& task) override;
};

}

#endif
