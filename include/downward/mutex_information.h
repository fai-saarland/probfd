#ifndef DOWNWARD_MUTEXES_H
#define DOWNWARD_MUTEXES_H

namespace downward {
struct FactPair;
}

namespace downward {

class MutexInformation {
public:
    virtual ~MutexInformation() = default;

    virtual bool
    are_facts_mutex(const FactPair& fact1, const FactPair& fact2) const = 0;
};

}

#endif
