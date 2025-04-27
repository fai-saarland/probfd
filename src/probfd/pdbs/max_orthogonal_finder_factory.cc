#include "probfd/pdbs/max_orthogonal_finder_factory.h"
#include "probfd/pdbs/max_orthogonal_finder.h"

namespace probfd::pdbs {

std::unique_ptr<SubCollectionFinder>
AdditiveMaxOrthogonalityFinderFactory::create_subcollection_finder(
    const SharedProbabilisticTask& task)
{
    return std::make_unique<AdditiveMaxOrthogonalityFinder>(
        get_variables(task),
        get_operators(task));
}

std::unique_ptr<SubCollectionFinder>
MultiplicativeMaxOrthogonalityFinderFactory::create_subcollection_finder(
    const SharedProbabilisticTask& task)
{
    return std::make_unique<MultiplicativeMaxOrthogonalityFinder>(
        get_variables(task),
        get_operators(task));
}

} // namespace probfd::pdbs
