#include "probfd/cli/merge_and_shrink/prune_strategy_identity.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/merge_and_shrink/prune_strategy_identity.h"

using namespace probfd::merge_and_shrink;
using namespace downward;
using namespace downward::cli::plugins;

namespace {
class PruneStrategyIdentityFeature : public SharedTypedFeature<PruneStrategy> {
public:
    PruneStrategyIdentityFeature()
        : TypedFeature("prune_identity", &PruneStrategyIdentityFeature::func)
    {
        document_title("Identity prune strategy");
        document_synopsis("This prune strategy leaves the TS unchanged.");
    }

    static std::shared_ptr<PruneStrategy> func()
    {
        return std::make_shared<PruneStrategyIdentity>();
    }
};
} // namespace

namespace probfd::cli::merge_and_shrink {

void add_prune_strategy_identity_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<PruneStrategyIdentityFeature>();
}

} // namespace probfd::cli::merge_and_shrink