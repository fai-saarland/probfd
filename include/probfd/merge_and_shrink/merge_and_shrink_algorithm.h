#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_H

#include "probfd/probabilistic_task.h"

#include "downward/utils/logging.h"

#include <memory>

namespace downward::utils {
class CountdownTimer;
}

namespace probfd::merge_and_shrink {
class FactoredTransitionSystem;
class LabelReduction;
class MergeStrategyFactory;
class MergeStrategy;
class ShrinkStrategy;
class PruneStrategy;
} // namespace probfd::merge_and_shrink

namespace probfd::merge_and_shrink {

class MergeAndShrinkAlgorithm {
    std::shared_ptr<MergeStrategyFactory> merge_strategy_factory;
    std::shared_ptr<ShrinkStrategy> shrink_strategy;
    std::shared_ptr<LabelReduction> label_reduction;
    std::shared_ptr<PruneStrategy> prune_strategy;

    // Options for shrinking
    // Hard limit: the maximum size of a transition system at any point.
    const int max_states;
    // Hard limit: the maximum size of a transition system before being merged.
    const int max_states_before_merge;
    /* A soft limit for triggering shrinking even if the hard limits
       max_states and max_states_before_merge are not violated. */
    const int shrink_threshold_before_merge;

    const downward::utils::Duration main_loop_max_time;

    downward::Kilobytes starting_peak_memory;

    void
    report_peak_memory_delta(downward::utils::LogProxy log, bool final = false)
        const;

    void dump_options(downward::utils::LogProxy log) const;

    void warn_on_unusual_options(downward::utils::LogProxy log) const;

    bool ran_out_of_time(
        const downward::utils::CountdownTimer& timer,
        downward::utils::LogProxy log) const;

    void
    statistics(downward::utils::LogProxy log, int maximum_intermediate_size)
        const;

    void main_loop(
        FactoredTransitionSystem& fts,
        MergeStrategy& merge_strategy,
        bool compute_liveness,
        bool compute_goal_distances,
        const downward::utils::CountdownTimer& timer,
        downward::utils::LogProxy log);

public:
    MergeAndShrinkAlgorithm(
        std::shared_ptr<MergeStrategyFactory> merge_strategy,
        std::shared_ptr<ShrinkStrategy> shrink_strategy,
        std::shared_ptr<LabelReduction> label_reduction,
        std::shared_ptr<PruneStrategy> prune_strategy,
        int max_states,
        int max_states_before_merge,
        int threshold_before_merge,
        downward::utils::Duration main_loop_max_time);

    FactoredTransitionSystem build_factored_transition_system(
        const SharedProbabilisticTask& task,
        downward::utils::LogProxy log);
};

} // namespace probfd::merge_and_shrink

#endif
