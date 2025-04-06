#include "probfd/merge_and_shrink/prune_strategy_alive.h"

#include "downward/cli/plugins/plugin.h"

using namespace probfd::merge_and_shrink;
using namespace downward;
using namespace downward::cli::plugins;

namespace {

class PruneStrategyAliveFeature
    : public TypedFeature<PruneStrategy, PruneStrategyAlive> {
public:
    PruneStrategyAliveFeature()
        : TypedFeature("prune_alive")
    {
        document_title("Alive states prune strategy");
        document_synopsis("This prune strategy keeps only alive states.");
    }

    std::shared_ptr<PruneStrategyAlive>
    create_component(const Options&, const utils::Context&)
        const override
    {
        return std::make_shared<PruneStrategyAlive>();
    }
};

FeaturePlugin<PruneStrategyAliveFeature> _plugin;

} // namespace probfd::merge_and_shrink