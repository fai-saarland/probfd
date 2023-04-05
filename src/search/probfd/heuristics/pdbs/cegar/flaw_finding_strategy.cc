#include "probfd/heuristics/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/heuristics/pdbs/maxprob_pattern_database.h"
#include "probfd/heuristics/pdbs/ssp_pattern_database.h"

#include "plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

static PluginTypePlugin<FlawFindingStrategy<SSPPatternDatabase>>
    _type_plugin_ec(
        "FlawFindingStrategy_ec",
        "Policy CEGAR flaw finding strategy for SSPs.");

static PluginTypePlugin<FlawFindingStrategy<MaxProbPatternDatabase>>
    _type_plugin_mp(
        "FlawFindingStrategy_mp",
        "Policy CEGAR flaw finding strategy for MaxProb.");

template class FlawFindingStrategy<MaxProbPatternDatabase>;
template class FlawFindingStrategy<SSPPatternDatabase>;

}
} // namespace pdbs
} // namespace heuristics
} // namespace probfd