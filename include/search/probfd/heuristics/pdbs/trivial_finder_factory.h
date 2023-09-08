#ifndef PROBFD_HEURISTICS_PDBS_TRIVIAL_FINDER_FACTORY_H
#define PROBFD_HEURISTICS_PDBS_TRIVIAL_FINDER_FACTORY_H

#include "probfd/heuristics/pdbs/subcollection_finder_factory.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

class TrivialFinderFactory : public SubCollectionFinderFactory {
public:
    std::unique_ptr<SubCollectionFinder>
        create_subcollection_finder(ProbabilisticTaskProxy) override;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __TRIVIAL_FINDER_H__