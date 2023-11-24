#ifndef PROBFD_PDBS_FULLY_ADDITIVE_FINDER_FACTORY_H
#define PROBFD_PDBS_FULLY_ADDITIVE_FINDER_FACTORY_H

#include "probfd/pdbs/subcollection_finder_factory.h"

namespace probfd {
namespace pdbs {

class FullyAdditiveFinderFactory : public SubCollectionFinderFactory {
public:
    std::unique_ptr<SubCollectionFinder> create_subcollection_finder(
        const ProbabilisticTaskProxy& task_proxy) override;
};

} // namespace pdbs
} // namespace probfd

#endif // PROBFD_PDBS_FULLY_ADDITIVE_FINDER_FACTORY_H
