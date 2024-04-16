#include "probfd/merge_and_shrink/pruning_strategy_alive.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "downward/utils/logging.h"

#include "downward/plugins/plugin.h"

namespace probfd::merge_and_shrink {

StateEquivalenceRelation PruningStrategyAlive::compute_pruning_abstraction(
    const TransitionSystem& ts,
    const Distances& distances,
    utils::LogProxy& log)
{
    int num_states = ts.get_size();
    StateEquivalenceRelation state_equivalence_relation;
    state_equivalence_relation.reserve(num_states);
    int non_alive = 0;
    for (int state = 0; state < num_states; ++state) {
        /* If pruning both unreachable and irrelevant states, a state which is
           dead is counted for both statistics! */
        assert(distances.is_liveness_computed());
        if (distances.is_alive(state)) {
            state_equivalence_relation.push_back({state});
        } else {
            ++non_alive;
        }
    }

    if (log.is_at_least_verbose() && non_alive > 0) {
        log << ts.tag() << "pruned: " << non_alive << " non-alive states"
            << std::endl;
    }

    return state_equivalence_relation;
}

bool PruningStrategyAlive::requires_liveness() const
{
    return true;
}

bool PruningStrategyAlive::requires_goal_distances() const
{
    return true;
}

namespace {

class PruningStrategyAliveFeature
    : public plugins::TypedFeature<PruningStrategy, PruningStrategyAlive> {
public:
    PruningStrategyAliveFeature()
        : TypedFeature("prune_alive")
    {
        document_title("Alive states prune strategy");
        document_synopsis("This prune strategy keeps only alive states.");
    }

    std::shared_ptr<PruningStrategyAlive>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<PruningStrategyAlive>();
    }
};

plugins::FeaturePlugin<PruningStrategyAliveFeature> _plugin;

} // namespace

} // namespace probfd::merge_and_shrink