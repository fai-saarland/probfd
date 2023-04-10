#include "probfd/heuristics/pdbs/max_orthogonal_finder_factory.h"
#include "probfd/heuristics/pdbs/max_orthogonal_finder.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

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

static std::shared_ptr<AdditiveMaxOrthogonalityFinderFactory>
_parse(OptionParser& parser)
{
    if (parser.dry_run()) return nullptr;
    return std::make_shared<AdditiveMaxOrthogonalityFinderFactory>();
}

static std::shared_ptr<MultiplicativeMaxOrthogonalityFinderFactory>
_parse2(OptionParser& parser)
{
    if (parser.dry_run()) return nullptr;
    return std::make_shared<MultiplicativeMaxOrthogonalityFinderFactory>();
}

static Plugin<SubCollectionFinderFactory>
    _plugin("additive_max_orthogonality_factory", _parse);

static Plugin<SubCollectionFinderFactory>
    _plugin2("multiplicative_max_orthogonality_factory", _parse2);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd