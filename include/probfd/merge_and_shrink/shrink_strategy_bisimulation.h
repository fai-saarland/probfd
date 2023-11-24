#ifndef PROBFD_MERGE_AND_SHRINK_SHRINK_STRATEGY_BISIMULATION_H
#define PROBFD_MERGE_AND_SHRINK_SHRINK_STRATEGY_BISIMULATION_H

#include "probfd/merge_and_shrink/shrink_strategy.h"

namespace downward::cli::plugins {
class Options;
}

namespace probfd::merge_and_shrink {
struct Signature;
}

namespace probfd::merge_and_shrink {

enum class AtLimit { RETURN, USE_UP };

class ShrinkBisimulation : public ShrinkStrategy {
    const AtLimit at_limit;

public:
    explicit ShrinkBisimulation(AtLimit at_limit);

    StateEquivalenceRelation compute_equivalence_relation(
        const TransitionSystem& ts,
        const Distances& distances,
        int target_size,
        utils::LogProxy& log) const override;

    bool requires_init_distances() const override { return false; }
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
