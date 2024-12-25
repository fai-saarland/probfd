#include "downward/cli/plugins/plugin.h"

#include "probfd/pdbs/cegar/flaw_finding_strategy.h"

using namespace probfd::pdbs::cegar;

using namespace downward::cli::plugins;

namespace {

class FlawFindingStrategyCategoryPlugin
    : public TypedCategoryPlugin<FlawFindingStrategy> {
public:
    FlawFindingStrategyCategoryPlugin()
        : TypedCategoryPlugin("FlawFindingStrategy")
    {
        document_synopsis("Policy CEGAR flaw finding strategy.");
    }
} _category_plugin;

} // namespace
