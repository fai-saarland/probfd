#include "probfd/heuristics/pdbs/pattern_selection/cegar/flaw_finding_strategy_factory.h"

#include "probfd/heuristics/pdbs/expcost_projection.h"
#include "probfd/heuristics/pdbs/maxprob_projection.h"

#include "plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace pattern_selection {

template class FlawFindingStrategyFactory<MaxProbProjection>;
template class FlawFindingStrategyFactory<ExpCostProjection>;

static PluginTypePlugin<FlawFindingStrategyFactory<ExpCostProjection>>
    _type_plugin_ec(
        "FlawFindingStrategyFactory_ec",
        "Policy CEGAR flaw finding strategy factory for SSPs.");

static PluginTypePlugin<FlawFindingStrategyFactory<MaxProbProjection>>
    _type_plugin_mp(
        "FlawFindingStrategyFactory_mp",
        "Policy CEGAR flaw finding strategy factory for MaxProb.");

} // namespace pattern_selection
} // namespace pdbs
} // namespace heuristics
} // namespace probfd