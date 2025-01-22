#ifndef D_MUTEXES_H
#define D_MUTEXES_H

#include "downward/abstract_task.h"

#include <memory>
#include <set>
#include <string>
#include <vector>

class AbstractTask;
struct FactPair;
class VariablesProxy;

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

class ExplicitMutexInformation : public MutexInformation {
    // TODO: think about using hash sets here.
    std::vector<std::vector<std::set<FactPair>>> mutexes_;

public:
    ExplicitMutexInformation(std::istream& in, const VariablesProxy& variables);

    bool are_facts_mutex(const FactPair& fact1, const FactPair& fact2)
        const override;
};

class FromFileMutexFactory : public MutexFactory {
    std::string filename;

public:
    explicit FromFileMutexFactory(std::string filename);

    std::shared_ptr<MutexInformation>
    compute_mutexes(const std::shared_ptr<AbstractTask>& task) override;
};

#endif // D_MUTEXES_H
