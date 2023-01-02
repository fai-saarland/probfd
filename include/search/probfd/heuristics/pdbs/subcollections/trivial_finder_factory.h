#ifndef MDPS_HEURISTICS_PDBS_SUBCOLLECTIONS_TRIVIAL_FINDER_FACTORY_H
#define MDPS_HEURISTICS_PDBS_SUBCOLLECTIONS_TRIVIAL_FINDER_FACTORY_H

#include "probfd/heuristics/pdbs/subcollections/subcollection_finder_factory.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

class TrivialFinderFactory : public SubCollectionFinderFactory {
public:
    std::unique_ptr<SubCollectionFinder>
    create_subcollection_finder(const ProbabilisticTaskProxy&) override;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __TRIVIAL_FINDER_H__