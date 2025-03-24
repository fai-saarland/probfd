#include "probfd/merge_and_shrink/shrink_strategy_identity.h"

#include "downward/cli/plugins/plugin.h"

using namespace std;
using namespace downward::cli::plugins;
using namespace probfd::merge_and_shrink;

namespace {

class ShrinkStrategyIdentityFeature
    : public TypedFeature<ShrinkStrategy, ShrinkStrategyIdentity> {
public:
    ShrinkStrategyIdentityFeature()
        : TypedFeature("shrink_identity")
    {
        document_title("distance-preserving shrink strategy");
        document_synopsis("This strategy applies no shrinking.");
    }

protected:
    shared_ptr<ShrinkStrategyIdentity>
    create_component(const Options&, const utils::Context&) const override
    {
        return make_shared_from_arg_tuples<ShrinkStrategyIdentity>();
    }
};

FeaturePlugin<ShrinkStrategyIdentityFeature> _plugin;

} // namespace probfd::merge_and_shrink