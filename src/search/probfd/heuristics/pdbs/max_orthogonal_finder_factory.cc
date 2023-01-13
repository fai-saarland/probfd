#include "probfd/heuristics/pdbs/max_orthogonal_finder_factory.h"
#include "probfd/heuristics/pdbs/max_orthogonal_finder.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

std::unique_ptr<SubCollectionFinder>
MaxOrthogonalityFinderFactory::create_subcollection_finder(
    const ProbabilisticTaskProxy& task_proxy)
{
    return std::make_unique<MaxOrthogonalityFinder>(task_proxy);
}

static std::shared_ptr<MaxOrthogonalityFinderFactory>
_parse(OptionParser& parser)
{
    if (parser.dry_run()) return nullptr;
    return std::make_shared<MaxOrthogonalityFinderFactory>();
}

static Plugin<SubCollectionFinderFactory>
    _plugin("finder_max_orthogonality_factory", _parse);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd