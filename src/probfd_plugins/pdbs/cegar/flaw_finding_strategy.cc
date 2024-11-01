#include "probfd/pdbs/cegar/flaw_finding_strategy.h"

#include "downward/plugins/plugin.h"

using namespace probfd::pdbs::cegar;

namespace {

class FlawFindingStrategyCategoryPlugin
    : public plugins::TypedCategoryPlugin<FlawFindingStrategy> {
public:
    FlawFindingStrategyCategoryPlugin()
        : TypedCategoryPlugin("FlawFindingStrategy")
    {
        document_synopsis("Policy CEGAR flaw finding strategy.");
    }
} _category_plugin;

} // namespace
