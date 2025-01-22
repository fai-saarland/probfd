#ifndef DOWNWARD_MUTEXES_H
#define DOWNWARD_MUTEXES_H

#include <memory>

namespace downward {
class AbstractTask;
struct FactPair;
}

namespace downward {

class MutexInformation {
public:
    virtual ~MutexInformation() = default;

    virtual bool
    are_facts_mutex(const FactPair& fact1, const FactPair& fact2) const = 0;
};

class MutexFactory {
public:
    virtual ~MutexFactory() = default;

    virtual std::shared_ptr<MutexInformation>
    compute_mutexes(const std::shared_ptr<AbstractTask>& task) = 0;
};

}

#endif
