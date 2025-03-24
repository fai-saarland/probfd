#ifndef PROBFD_MERGE_AND_SHRINK_SHRINK_STRATEGY_PROBABILISTIC_BISIMULATION_H
#define PROBFD_MERGE_AND_SHRINK_SHRINK_STRATEGY_PROBABILISTIC_BISIMULATION_H

#include "probfd/merge_and_shrink/shrink_strategy.h"

namespace probfd::merge_and_shrink {

class ShrinkStrategyProbabilisticBisimulation : public ShrinkStrategy {
public:
    enum class AtLimit { RETURN, USE_UP };

private:
    const AtLimit at_limit;
    const bool require_goal_distances;

public:
    ShrinkStrategyProbabilisticBisimulation(
        AtLimit at_limit,
        bool require_goal_distances);

    StateEquivalenceRelation compute_equivalence_relation(
        const TransitionSystem& ts,
        const Distances& distances,
        int target_size,
        utils::LogProxy& log) const override;

    bool requires_liveness() const override { return false; }
    bool requires_goal_distances() const override { return true; }

protected:
    void dump_strategy_specific_options(utils::LogProxy& log) const override;

    std::string name() const override;

private:
    void compute_abstraction(
        const TransitionSystem& ts,
        const Distances& distances,
        int target_size,
        StateEquivalenceRelation& equivalence_relation) const;
};

} // namespace probfd::merge_and_shrink

#endif
