#ifndef PROBFD_HEURISTICS_PDBS_MAX_ORTHOGONAL_FINDER_FACTORY_H
#define PROBFD_HEURISTICS_PDBS_MAX_ORTHOGONAL_FINDER_FACTORY_H

#include "probfd/heuristics/pdbs/subcollection_finder_factory.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

class AdditiveMaxOrthogonalityFinderFactory
    : public SubCollectionFinderFactory {
public:
    std::unique_ptr<SubCollectionFinder> create_subcollection_finder(
        const ProbabilisticTaskProxy& task_proxy) override;
};

class MultiplicativeMaxOrthogonalityFinderFactory
    : public SubCollectionFinderFactory {
public:
    std::unique_ptr<SubCollectionFinder> create_subcollection_finder(
        const ProbabilisticTaskProxy& task_proxy) override;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
#endif // __MAX_ORTHOGONAL_FINDER_H__