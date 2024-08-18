#include "probfd/pdbs/max_orthogonal_finder_factory.h"
#include "probfd/pdbs/max_orthogonal_finder.h"

namespace probfd::pdbs {

std::unique_ptr<SubCollectionFinder>
AdditiveMaxOrthogonalityFinderFactory::create_subcollection_finder(
    const ProbabilisticTaskProxy& task_proxy)
{
    return std::make_unique<AdditiveMaxOrthogonalityFinder>(task_proxy);
}

std::unique_ptr<SubCollectionFinder>
MultiplicativeMaxOrthogonalityFinderFactory::create_subcollection_finder(
    const ProbabilisticTaskProxy& task_proxy)
{
    return std::make_unique<MultiplicativeMaxOrthogonalityFinder>(task_proxy);
}

} // namespace probfd::pdbs
