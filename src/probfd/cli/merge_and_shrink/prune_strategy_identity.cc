#include "probfd/cli/merge_and_shrink/prune_strategy_identity.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/merge_and_shrink/prune_strategy_identity.h"

using namespace probfd::merge_and_shrink;
using namespace downward;
using namespace downward::cli::plugins;

namespace {
class PruneStrategyIdentityFeature : public TypedFeature<PruneStrategy> {
public:
    PruneStrategyIdentityFeature()
        : TypedFeature("prune_identity")
    {
        document_title("Identity prune strategy");
        document_synopsis("This prune strategy leaves the TS unchanged.");
    }

    std::shared_ptr<PruneStrategy>
    create_component(const Options&, const utils::Context&) const override
    {
        return std::make_shared<PruneStrategyIdentity>();
    }
};
} // namespace

namespace probfd::cli::merge_and_shrink {

void add_prune_strategy_identity_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<PruneStrategyIdentityFeature>();
}

} // namespace probfd::cli::merge_and_shrink