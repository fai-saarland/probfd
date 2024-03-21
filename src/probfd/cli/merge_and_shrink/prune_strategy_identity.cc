#include "probfd/merge_and_shrink/prune_strategy_identity.h"

#include "downward/cli/plugins/plugin.h"

using namespace probfd::merge_and_shrink;
using namespace downward::cli::plugins;

namespace {

class PruneStrategyIdentityFeature
    : public TypedFeature<PruneStrategy, PruneStrategyIdentity> {
public:
    PruneStrategyIdentityFeature()
        : TypedFeature("prune_identity")
    {
        document_title("Identity prune strategy");
        document_synopsis("This prune strategy leaves the TS unchanged.");
    }

    std::shared_ptr<PruneStrategyIdentity>
    create_component(const Options&, const utils::Context&)
        const override
    {
        return std::make_shared<PruneStrategyIdentity>();
    }
} _plugin;

} // namespace probfd::merge_and_shrink