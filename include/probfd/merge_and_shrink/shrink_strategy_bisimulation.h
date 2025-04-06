#ifndef PROBFD_MERGE_AND_SHRINK_SHRINK_STRATEGY_BISIMULATION_H
#define PROBFD_MERGE_AND_SHRINK_SHRINK_STRATEGY_BISIMULATION_H

#include "probfd/merge_and_shrink/shrink_strategy.h"

namespace downward::cli::plugins {
class Options;
}

namespace probfd::merge_and_shrink {

class ShrinkStrategyBisimulation : public ShrinkStrategy {
public:
    enum class AtLimit { RETURN, USE_UP };

private:
    const AtLimit at_limit;
    const bool require_goal_distances;

public:
    ShrinkStrategyBisimulation(
        AtLimit at_limit,
        bool require_goal_distances);

    StateEquivalenceRelation compute_equivalence_relation(
        const Labels& labels,
        const TransitionSystem& ts,
        const Distances& distances,
        int target_size,
        downward::utils::LogProxy& log) const override;

    bool requires_liveness() const override { return false; }

    bool requires_goal_distances() const override { return true; }

protected:
    void dump_strategy_specific_options(
        downward::utils::LogProxy& log) const override;

    std::string name() const override;

private:
    void compute_abstraction(
        const Labels& labels,
        const TransitionSystem& ts,
        const Distances& distances,
        int target_size,
        StateEquivalenceRelation& equivalence_relation) const;
};

} // namespace probfd::merge_and_shrink

#endif
