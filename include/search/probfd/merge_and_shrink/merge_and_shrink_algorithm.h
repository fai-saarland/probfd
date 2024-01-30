#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_H

#include "downward/utils/logging.h"

#include <memory>

namespace plugins {
class Feature;
class Options;
} // namespace plugins

namespace utils {
class CountdownTimer;
}

namespace probfd {
class ProbabilisticTaskProxy;
class ProbabilisticTask;
}

namespace probfd::merge_and_shrink {
class FactoredTransitionSystem;
class LabelReduction;
class MergeStrategyFactory;
class MergeStrategy;
class ShrinkStrategy;
class PruningStrategy;
} // namespace probfd::merge_and_shrink

namespace probfd::merge_and_shrink {

class MergeAndShrinkAlgorithm {
    // TODO: when the option parser supports it, the following should become
    // unique pointers.
    std::shared_ptr<MergeStrategyFactory> merge_strategy_factory;
    std::shared_ptr<ShrinkStrategy> shrink_strategy;
    std::shared_ptr<PruningStrategy> pruning_strategy;
    std::shared_ptr<LabelReduction> label_reduction;

    // Options for shrinking
    // Hard limit: the maximum size of a transition system at any point.
    const int max_states;
    // Hard limit: the maximum size of a transition system before being merged.
    const int max_states_before_merge;
    /* A soft limit for triggering shrinking even if the hard limits
       max_states and max_states_before_merge are not violated. */
    const int shrink_threshold_before_merge;

    const double main_loop_max_time;

    long starting_peak_memory;

    mutable utils::LogProxy log;

    void report_peak_memory_delta(bool final = false) const;
    void dump_options() const;
    void warn_on_unusual_options() const;
    void statistics(int maximum_intermediate_size) const;
    void main_loop(
        FactoredTransitionSystem& fts,
        MergeStrategy& merge_strategy,
        utils::CountdownTimer& timer);

public:
    explicit MergeAndShrinkAlgorithm(const plugins::Options& opts);
    FactoredTransitionSystem build_factored_transition_system(
        std::shared_ptr<ProbabilisticTask>& task_proxy);
};

extern void
add_merge_and_shrink_algorithm_options_to_feature(plugins::Feature& feature);
extern void
add_transition_system_size_limit_options_to_feature(plugins::Feature& feature);
extern void handle_shrink_limit_options_defaults(
    plugins::Options& opts,
    const utils::Context& context);

} // namespace probfd::merge_and_shrink

#endif // PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_H
