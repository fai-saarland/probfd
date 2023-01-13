#include "probfd/heuristics/pdbs/cegar/pucs_flaw_finder_factory.h"
#include "probfd/heuristics/pdbs/cegar/pucs_flaw_finder.h"

#include "probfd/heuristics/pdbs/expcost_projection.h"
#include "probfd/heuristics/pdbs/maxprob_projection.h"

#include "utils/collections.h"

#include "option_parser.h"
#include "plugin.h"

using namespace std;
using namespace utils;

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

template <typename PDBType>
PUCSFlawFinderFactory<PDBType>::PUCSFlawFinderFactory(
    const options::Options& opts)
    : PUCSFlawFinderFactory<PDBType>(opts.get<int>("violation_threshold"))
{
}

template <typename PDBType>
PUCSFlawFinderFactory<PDBType>::PUCSFlawFinderFactory(
    unsigned int violation_threshold)
    : violation_threshold(violation_threshold)
{
}

template <typename PDBType>
std::unique_ptr<FlawFindingStrategy<PDBType>>
PUCSFlawFinderFactory<PDBType>::create_flaw_finder(
    const ProbabilisticTask* task)
{
    return std::make_unique<PUCSFlawFinder<PDBType>>(task, violation_threshold);
}

template <typename PDBType>
static std::shared_ptr<FlawFindingStrategyFactory<PDBType>>
_parse(options::OptionParser& parser)
{
    parser.add_option<int>(
        "violation_threshold",
        "Maximal number of states for which a flaw is tolerated before aborting"
        "the search.",
        "1",
        options::Bounds("0", "infinity"));

    Options opts = parser.parse();
    if (parser.dry_run()) return nullptr;

    return make_shared<PUCSFlawFinderFactory<PDBType>>(opts);
}

static Plugin<FlawFindingStrategyFactory<MaxProbProjection>>
    _plugin_maxprob("pucs_flaw_finder_factory_mp", _parse<MaxProbProjection>);
static Plugin<FlawFindingStrategyFactory<ExpCostProjection>>
    _plugin_expcost("pucs_flaw_finder_factory_ec", _parse<ExpCostProjection>);

template class PUCSFlawFinderFactory<MaxProbProjection>;
template class PUCSFlawFinderFactory<ExpCostProjection>;

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd