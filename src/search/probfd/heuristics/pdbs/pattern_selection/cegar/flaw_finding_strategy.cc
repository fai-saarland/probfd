#include "probfd/heuristics/pdbs/pattern_selection/cegar/flaw_finding_strategy.h"

#include "probfd/heuristics/pdbs/expcost_projection.h"
#include "probfd/heuristics/pdbs/maxprob_projection.h"

#include "plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace pattern_selection {

template class FlawFindingStrategy<MaxProbProjection>;
template class FlawFindingStrategy<ExpCostProjection>;

static PluginTypePlugin<FlawFindingStrategy<ExpCostProjection>> _type_plugin_ec(
    "FlawFindingStrategy_ec",
    "Policy CEGAR flaw finding strategy for SSPs");

static PluginTypePlugin<FlawFindingStrategy<MaxProbProjection>> _type_plugin_mp(
    "FlawFindingStrategy_mp",
    "Policy CEGAR flaw finding strategy for MaxProb setting");

} // namespace pattern_selection
} // namespace pdbs
} // namespace heuristics
} // namespace probfd