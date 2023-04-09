#include "probfd/heuristics/pdbs/cegar/flaw_finding_strategy.h"

#include "plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

static PluginTypePlugin<FlawFindingStrategy>
    _type_plugin("FlawFindingStrategy", "Policy CEGAR flaw finding strategy.");

}
} // namespace pdbs
} // namespace heuristics
} // namespace probfd