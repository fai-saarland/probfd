#include "probfd/heuristics/pdbs/cegar/sampling_flaw_finder_factory.h"
#include "probfd/heuristics/pdbs/cegar/sampling_flaw_finder.h"

#include "probfd/heuristics/pdbs/maxprob_pattern_database.h"
#include "probfd/heuristics/pdbs/ssp_pattern_database.h"

#include "option_parser.h"
#include "plugin.h"

using namespace std;

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

template <typename PDBType>
SamplingFlawFinderFactory<PDBType>::SamplingFlawFinderFactory(
    options::Options& opts)
    : SamplingFlawFinderFactory<PDBType>(opts.get<int>("violation_threshold"))
{
}

template <typename PDBType>
SamplingFlawFinderFactory<PDBType>::SamplingFlawFinderFactory(
    unsigned int violation_threshold)
    : violation_threshold(violation_threshold)
{
}

template <typename PDBType>
std::unique_ptr<FlawFindingStrategy<PDBType>>
SamplingFlawFinderFactory<PDBType>::create_flaw_finder(
    const ProbabilisticTask* task)
{
    return std::make_unique<SamplingFlawFinder<PDBType>>(
        task,
        violation_threshold);
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
        options::Bounds("1", "infinity"));

    Options opts = parser.parse();
    if (parser.dry_run()) return nullptr;

    return make_shared<SamplingFlawFinderFactory<PDBType>>(opts);
}

static Plugin<FlawFindingStrategyFactory<MaxProbPatternDatabase>>
    _plugin_maxprob(
        "sampling_flaw_finder_factory_mp",
        _parse<MaxProbPatternDatabase>);
static Plugin<FlawFindingStrategyFactory<SSPPatternDatabase>> _plugin_expcost(
    "sampling_flaw_finder_factory_ec",
    _parse<SSPPatternDatabase>);

template class SamplingFlawFinderFactory<MaxProbPatternDatabase>;
template class SamplingFlawFinderFactory<SSPPatternDatabase>;

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd