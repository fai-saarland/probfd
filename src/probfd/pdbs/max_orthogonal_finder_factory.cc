#include "probfd/pdbs/max_orthogonal_finder_factory.h"
#include "probfd/pdbs/max_orthogonal_finder.h"

namespace probfd::pdbs {

std::unique_ptr<SubCollectionFinder>
AdditiveMaxOrthogonalityFinderFactory::create_subcollection_finder(
    const ProbabilisticTask& task)
{
    return std::make_unique<AdditiveMaxOrthogonalityFinder>(task);
}

std::unique_ptr<SubCollectionFinder>
MultiplicativeMaxOrthogonalityFinderFactory::create_subcollection_finder(
    const ProbabilisticTask& task)
{
    return std::make_unique<MultiplicativeMaxOrthogonalityFinder>(task);
}

} // namespace probfd::pdbs
