#ifndef PROBFD_PDBS_SUBCOLLECTION_FINDER_FACTORY_H
#define PROBFD_PDBS_SUBCOLLECTION_FINDER_FACTORY_H

#include <memory>

namespace probfd {
class ProbabilisticTaskProxy;

namespace pdbs {

class SubCollectionFinder;

class SubCollectionFinderFactory {
public:
    virtual ~SubCollectionFinderFactory() = default;

    virtual std::unique_ptr<SubCollectionFinder>
    create_subcollection_finder(const ProbabilisticTaskProxy& task_proxy) = 0;
};

} // namespace pdbs
} // namespace probfd

#endif // PROBFD_PDBS_SUBCOLLECTION_FINDER_FACTORY_H
