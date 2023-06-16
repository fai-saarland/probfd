#include "probfd/heuristics/pdbs/cegar/flaw_finding_strategy.h"

#include "plugins/plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

static class FlawFindingStrategyCategoryPlugin
    : public plugins::TypedCategoryPlugin<FlawFindingStrategy> {
public:
    FlawFindingStrategyCategoryPlugin()
        : TypedCategoryPlugin("FlawFindingStrategy")
    {
        document_synopsis("Policy CEGAR flaw finding strategy.");
    }
} _category_plugin;
}
} // namespace pdbs
} // namespace heuristics
} // namespace probfd