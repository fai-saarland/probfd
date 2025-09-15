#include "probfd/pdbs/fully_additive_finder_factory.h"
#include "probfd/pdbs/fully_additive_finder.h"

namespace probfd::pdbs {

std::unique_ptr<SubCollectionFinder>
FullyAdditiveFinderFactory::create_subcollection_finder(
    const SharedProbabilisticTask&)
{
    return std::make_unique<FullyAdditiveFinder>();
}

} // namespace probfd::pdbs
