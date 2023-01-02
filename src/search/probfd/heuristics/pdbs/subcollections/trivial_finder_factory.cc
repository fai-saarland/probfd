#include "probfd/heuristics/pdbs/subcollections/trivial_finder_factory.h"
#include "probfd/heuristics/pdbs/subcollections/trivial_finder.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

std::unique_ptr<SubCollectionFinder>
TrivialFinderFactory::create_subcollection_finder(const ProbabilisticTaskProxy&)
{
    return std::make_unique<TrivialFinder>();
}

static std::shared_ptr<TrivialFinderFactory> _parse(OptionParser& parser)
{
    if (parser.dry_run()) return nullptr;
    return std::make_shared<TrivialFinderFactory>();
}

static Plugin<SubCollectionFinderFactory>
    _plugin("finder_trivial_factory", _parse);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd