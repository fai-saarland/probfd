#include "flaw_finding_strategy.h"

#include "../../expcost_projection.h"
#include "../../maxprob_projection.h"

#include "../../../../../plugin.h"

namespace probabilistic {
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
} // namespace probabilistic