#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_SHRINK_PROBABILISTIC_BISIMULATION_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_SHRINK_PROBABILISTIC_BISIMULATION_H

#include "probfd/merge_and_shrink/shrink_strategy.h"

namespace plugins {
class Options;
}

namespace probfd::merge_and_shrink {

class ShrinkProbabilisticBisimulation : public ShrinkStrategy {
public:
    enum class AtLimit { RETURN, USE_UP };

private:
    const AtLimit at_limit;
    const bool require_goal_distances;

    struct Signature;

public:
    explicit ShrinkProbabilisticBisimulation(const plugins::Options& opts);
    ShrinkProbabilisticBisimulation(
        AtLimit at_limit,
        bool require_goal_distances);

    StateEquivalenceRelation compute_equivalence_relation(
        const TransitionSystem& ts,
        const Distances& distances,
        int target_size,
        utils::LogProxy& log) const override;

    bool requires_liveness() const override;
    bool requires_goal_distances() const override;

protected:
    virtual void
    dump_strategy_specific_options(utils::LogProxy& log) const override;

    virtual std::string name() const override;

private:
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
};

} // namespace probfd::merge_and_shrink

#endif // PROBFD_HEURISTICS_MERGE_AND_SHRINK_SHRINK_PROBABILISTIC_BISIMULATION_H
