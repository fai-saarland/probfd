#include "probfd_cli/merge_and_shrink/prune_strategy_alive.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "probfd/merge_and_shrink/prune_strategy_alive.h"

using namespace probfd::merge_and_shrink;
using namespace downward;

using namespace language;
using namespace language::plugins;

namespace {
class PruneStrategyAliveFeature : public TypedFeature<PruneStrategy> {
public:
    PruneStrategyAliveFeature()
        : TypedFeature("prune_alive")
    {
        document_title("Alive states prune strategy");
        document_synopsis("This prune strategy keeps only alive states.");
    }

    std::shared_ptr<PruneStrategy>
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<PruneStrategyAlive>();
    }
};
} // namespace

namespace probfd::cli::merge_and_shrink {

void add_prune_strategy_alive_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<PruneStrategyAliveFeature>();
}

} // namespace probfd::cli::merge_and_shrink