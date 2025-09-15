#ifndef PROBFD_PDBS_MAX_ORTHOGONAL_FINDER_FACTORY_H
#define PROBFD_PDBS_MAX_ORTHOGONAL_FINDER_FACTORY_H

#include "probfd/pdbs/subcollection_finder_factory.h"

#include <memory>

namespace probfd::pdbs {

class AdditiveMaxOrthogonalityFinderFactory
    : public SubCollectionFinderFactory {
public:
    std::unique_ptr<SubCollectionFinder> create_subcollection_finder(
        const SharedProbabilisticTask& task) override;
};

class MultiplicativeMaxOrthogonalityFinderFactory
    : public SubCollectionFinderFactory {
public:
    std::unique_ptr<SubCollectionFinder> create_subcollection_finder(
        const SharedProbabilisticTask& task) override;
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_MAX_ORTHOGONAL_FINDER_FACTORY_H
