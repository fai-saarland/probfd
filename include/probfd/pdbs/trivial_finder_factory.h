#ifndef PROBFD_PDBS_TRIVIAL_FINDER_FACTORY_H
#define PROBFD_PDBS_TRIVIAL_FINDER_FACTORY_H

#include "probfd/pdbs/subcollection_finder_factory.h"

#include <memory>

namespace probfd::pdbs {

class TrivialFinderFactory : public SubCollectionFinderFactory {
public:
    std::unique_ptr<SubCollectionFinder>
    create_subcollection_finder(const ProbabilisticTask&) override;
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_TRIVIAL_FINDER_FACTORY_H
