#ifndef PROBFD_PDBS_SUBCOLLECTION_FINDER_FACTORY_H
#define PROBFD_PDBS_SUBCOLLECTION_FINDER_FACTORY_H

#include <memory>

// Forward Declarations
namespace probfd {
class ProbabilisticTaskProxy;
}

namespace probfd::pdbs {
class SubCollectionFinder;
}

namespace probfd::pdbs {
class SubCollectionFinderFactory {
public:
    virtual ~SubCollectionFinderFactory() = default;

    virtual std::unique_ptr<SubCollectionFinder>
    create_subcollection_finder(const ProbabilisticTaskProxy& task_proxy) = 0;
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_SUBCOLLECTION_FINDER_FACTORY_H
