#include "probfd/merge_and_shrink/pruning_strategy_identity.h"

#include "probfd/merge_and_shrink/transition_system.h"

#include "downward/plugins/plugin.h"

namespace probfd::merge_and_shrink {

StateEquivalenceRelation PruningStrategyIdentity::compute_pruning_abstraction(
    const TransitionSystem& ts,
    const Distances&,
    utils::LogProxy&)

{
    StateEquivalenceRelation relation;
    for (int index = 0; index != ts.get_size(); ++index) {
        relation.push_back({index});
    }
    return relation;
}

bool PruningStrategyIdentity::requires_liveness() const
{
    return false;
}

bool PruningStrategyIdentity::requires_goal_distances() const
{
    return false;
}

void PruningStrategyIdentity::dump_options(utils::LogProxy&) const
{
    // Nothing to report.
}

namespace {

class PruningStrategyIdentityFeature
    : public plugins::TypedFeature<PruningStrategy, PruningStrategyIdentity> {
public:
    PruningStrategyIdentityFeature()
        : TypedFeature("prune_identity")
    {
        document_title("Identity prune strategy");
        document_synopsis("This prune strategy leaves the TS unchanged.");
    }

    std::shared_ptr<PruningStrategyIdentity>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<PruningStrategyIdentity>();
    }
};

plugins::FeaturePlugin<PruningStrategyIdentityFeature> _plugin;

} // namespace

} // namespace probfd::merge_and_shrink