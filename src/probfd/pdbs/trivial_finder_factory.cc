#include "probfd/pdbs/trivial_finder_factory.h"
#include "probfd/pdbs/trivial_finder.h"

namespace probfd::pdbs {

std::unique_ptr<SubCollectionFinder>
TrivialFinderFactory::create_subcollection_finder(
    const SharedProbabilisticTask&)
{
    return std::make_unique<TrivialFinder>();
}

} // namespace probfd::pdbs
