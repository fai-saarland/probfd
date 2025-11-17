#include "probfd/cli/merge_and_shrink/prune_strategy_alive.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/merge_and_shrink/prune_strategy_alive.h"

using namespace probfd::merge_and_shrink;
using namespace downward;
using namespace downward::cli::plugins;

namespace {
class PruneStrategyAliveFeature : public SharedTypedFeature<PruneStrategy> {
public:
    PruneStrategyAliveFeature()
        : TypedFeature("prune_alive", &PruneStrategyAliveFeature::func)
    {
        document_title("Alive states prune strategy");
        document_synopsis("This prune strategy keeps only alive states.");
    }

    static std::shared_ptr<PruneStrategy> func(const utils::Context&)
    {
        return std::make_shared<PruneStrategyAlive>();
    }
};
} // namespace

namespace probfd::cli::merge_and_shrink {

void add_prune_strategy_alive_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<PruneStrategyAliveFeature>();
}

} // namespace probfd::cli::merge_and_shrink