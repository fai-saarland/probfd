#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_PRUNING_STRATEGY_SOLVABLE_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_PRUNING_STRATEGY_SOLVABLE_H

#include "probfd/merge_and_shrink/pruning_strategy.h"

namespace utils {
class LogProxy;
}

namespace probfd::merge_and_shrink {
class Distances;
class TransitionSystem;
} // namespace probfd::merge_and_shrink

namespace probfd::merge_and_shrink {

class PruningStrategySolvable : public PruningStrategy {
public:
    StateEquivalenceRelation compute_pruning_abstraction(
        const TransitionSystem& ts,
        const Distances& distances,
        utils::LogProxy& log) override;

    bool requires_liveness() const override;
    bool requires_goal_distances() const override;

    void dump_options(utils::LogProxy&) const override {}
};

} // namespace probfd::merge_and_shrink

#endif // PROBFD_HEURISTICS_MERGE_AND_SHRINK_PRUNING_STRATEGY_H
