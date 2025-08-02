#include "probfd/cli/pdbs/cegar/flaw_finding_strategy_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

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
};
}

namespace probfd::cli::pdbs::cegar {

void add_flaw_finding_strategy_category(RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugin<FlawFindingStrategyCategoryPlugin>();
}

} // namespace
