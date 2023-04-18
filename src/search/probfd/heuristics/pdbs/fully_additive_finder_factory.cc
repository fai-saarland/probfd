#include "probfd/heuristics/pdbs/fully_additive_finder.h"
#include "probfd/heuristics/pdbs/fully_additive_finder_factory.h"


#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

std::unique_ptr<SubCollectionFinder>
FullyAdditiveFinderFactory::create_subcollection_finder(
    const ProbabilisticTaskProxy&)
{
    return std::make_unique<FullyAdditiveFinder>();
}

static std::shared_ptr<FullyAdditiveFinderFactory> _parse(OptionParser& parser)
{
    if (parser.dry_run()) return nullptr;
    return std::make_shared<FullyAdditiveFinderFactory>();
}

static Plugin<SubCollectionFinderFactory>
    _plugin("fully_additive_factory", _parse);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd