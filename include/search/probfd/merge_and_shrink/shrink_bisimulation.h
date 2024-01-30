#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_SHRINK_BISIMULATION_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_SHRINK_BISIMULATION_H

#include "probfd/merge_and_shrink/shrink_strategy.h"

namespace plugins {
class Options;
}

namespace probfd::merge_and_shrink {
struct Signature;
}

namespace probfd::merge_and_shrink {

enum class AtLimit { RETURN, USE_UP };

class ShrinkBisimulation : public ShrinkStrategy {
    const AtLimit at_limit;

    void compute_abstraction(
        const TransitionSystem& ts,
        const Distances& distances,
        int target_size,
        StateEquivalenceRelation& equivalence_relation) const;

    int initialize_groups(
        const TransitionSystem& ts,
        const Distances& distances,
        std::vector<int>& state_to_group) const;

    void compute_signatures(
        const TransitionSystem& ts,
        std::vector<Signature>& signatures,
        const std::vector<int>& state_to_group) const;

public:
    explicit ShrinkBisimulation(const plugins::Options& opts);

    StateEquivalenceRelation compute_equivalence_relation(
        const TransitionSystem& ts,
        const Distances& distances,
        int target_size,
        utils::LogProxy& log) const override;

    bool requires_liveness() const override { return false; }
    bool requires_goal_distances() const override { return true; }

protected:
    virtual void
    dump_strategy_specific_options(utils::LogProxy& log) const override;
    virtual std::string name() const override;
};

} // namespace probfd::merge_and_shrink

#endif // PROBFD_HEURISTICS_MERGE_AND_SHRINK_SHRINK_BISIMULATION_H
