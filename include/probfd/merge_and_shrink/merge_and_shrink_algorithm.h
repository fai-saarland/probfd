#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_H

#include "downward/utils/logging.h"

#include <memory>

namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace utils {
class CountdownTimer;
}

namespace probfd {
class ProbabilisticTaskProxy;
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

    // Options for pruning
    const bool prune_unreachable_states;
    const bool prune_irrelevant_states;

    const double main_loop_max_time;

    long starting_peak_memory;

    void
    report_peak_memory_delta(utils::LogProxy log, bool final = false) const;

    void dump_options(utils::LogProxy log) const;

    void warn_on_unusual_options(utils::LogProxy log) const;

    bool
    ran_out_of_time(const utils::CountdownTimer& timer, utils::LogProxy log)
        const;

    void statistics(utils::LogProxy log, int maximum_intermediate_size) const;

    void main_loop(
        FactoredTransitionSystem& fts,
        MergeStrategy& merge_strategy,
        const utils::CountdownTimer& timer,
        utils::LogProxy log);

public:
    MergeAndShrinkAlgorithm(
        std::shared_ptr<MergeStrategyFactory> merge_strategy,
        std::shared_ptr<ShrinkStrategy> shrink_strategy,
        std::shared_ptr<LabelReduction> label_reduction,
        std::shared_ptr<PruneStrategy> prune_strategy,
        int max_states,
        int max_states_before_merge,
        int threshold_before_merge,
        bool prune_unreachable_states,
        bool prune_irrelevant_states,
        double main_loop_max_time);

    FactoredTransitionSystem build_factored_transition_system(
        const ProbabilisticTaskProxy& task_proxy,
        utils::LogProxy log);
};

extern void add_merge_and_shrink_algorithm_options_to_feature(
    downward::cli::plugins::Feature& feature);
extern void add_transition_system_size_limit_options_to_feature(
    downward::cli::plugins::Feature& feature);
extern void handle_shrink_limit_options_defaults(
    downward::cli::plugins::Options& opts,
    const utils::Context& context);

} // namespace probfd::merge_and_shrink

#endif
