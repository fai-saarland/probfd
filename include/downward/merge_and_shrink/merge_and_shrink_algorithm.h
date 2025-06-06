#ifndef MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_H
#define MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_H

#include "downward/utils/logging.h"

#include <memory>

namespace downward {
class TaskProxy;
}

namespace downward::utils {
class CountdownTimer;
}

namespace downward::merge_and_shrink {
class FactoredTransitionSystem;
class LabelReduction;
class MergeStrategyFactory;
class ShrinkStrategy;

class MergeAndShrinkAlgorithm {
    // TODO: when the option parser supports it, the following should become
    // unique pointers.
    std::shared_ptr<MergeStrategyFactory> merge_strategy_factory;
    std::shared_ptr<ShrinkStrategy> shrink_strategy;
    std::shared_ptr<LabelReduction> label_reduction;

    // Options for shrinking
    // Hard limit: the maximum size of a transition system at any point.
    const int max_states;
    // Hard limit: the maximum size of a transition system before being merged.
    const int max_states_before_merge;
    /* A soft limit for triggering shrinking even if the hard limits
       max_states and max_states_before_merge are not violated. */
    const int shrink_threshold_before_merge;

    // Options for pruning
    const bool prune_unreachable_states;
    const bool prune_irrelevant_states;

    mutable utils::LogProxy log;
    const double main_loop_max_time;

    long starting_peak_memory;

    void report_peak_memory_delta(bool final = false) const;
    void dump_options() const;
    void warn_on_unusual_options() const;
    bool ran_out_of_time(const utils::CountdownTimer& timer) const;
    void statistics(int maximum_intermediate_size) const;
    void main_loop(FactoredTransitionSystem& fts, const TaskProxy& task_proxy);

public:
    MergeAndShrinkAlgorithm(
        const std::shared_ptr<MergeStrategyFactory>& merge_strategy,
        const std::shared_ptr<ShrinkStrategy>& shrink_strategy,
        const std::shared_ptr<LabelReduction>& label_reduction,
        bool prune_unreachable_states,
        bool prune_irrelevant_states,
        int max_states,
        int max_states_before_merge,
        int threshold_before_merge,
        double main_loop_max_time,
        utils::Verbosity verbosity);
    FactoredTransitionSystem
    build_factored_transition_system(const TaskProxy& task_proxy);
};

} // namespace merge_and_shrink

#endif
