#include "probfd/heuristics/pdbs/cegar/flaw_finding_strategy_factory.h"

#include "probfd/heuristics/pdbs/maxprob_pattern_database.h"
#include "probfd/heuristics/pdbs/ssp_pattern_database.h"

#include "plugin.h"

#include <limits>

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

template class FlawFindingStrategyFactory<MaxProbPatternDatabase>;
template class FlawFindingStrategyFactory<SSPPatternDatabase>;

static PluginTypePlugin<FlawFindingStrategyFactory<SSPPatternDatabase>>
    _type_plugin_ec(
        "FlawFindingStrategyFactory_ec",
        "Policy CEGAR flaw finding strategy factory for SSPs.");

static PluginTypePlugin<FlawFindingStrategyFactory<MaxProbPatternDatabase>>
    _type_plugin_mp(
        "FlawFindingStrategyFactory_mp",
        "Policy CEGAR flaw finding strategy factory for MaxProb.");
}
} // namespace pdbs
} // namespace heuristics
} // namespace probfd