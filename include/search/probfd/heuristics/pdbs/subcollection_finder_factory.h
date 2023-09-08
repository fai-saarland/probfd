#ifndef PROBFD_HEURISTICS_PDBS_SUBCOLLECTION_FINDER_FACTORY_H
#define PROBFD_HEURISTICS_PDBS_SUBCOLLECTION_FINDER_FACTORY_H

#include "probfd/task_proxy.h"

#include <memory>

namespace probfd {
namespace heuristics {
namespace pdbs {

class SubCollectionFinder;

class SubCollectionFinderFactory {
public:
    virtual ~SubCollectionFinderFactory() = default;

    virtual std::unique_ptr<SubCollectionFinder>
    create_subcollection_finder(ProbabilisticTaskProxy task_proxy) = 0;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
#endif // __SUBCOLLECTION_FINDER_H__