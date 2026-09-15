#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_AND_SHRINK_ALGORITHM_H

#include "probfd/probabilistic_task.h"

#include "downward/utils/logging.h"

#include <memory>

namespace downward::utils {
class CountdownTimer;
class LogProxy;
} // namespace downward::utils

namespace probfd::merge_and_shrink {
class FactoredTransitionSystem;
class LabelReduction;
class MergeStrategyFactory;
class MergeStrategy;
class ShrinkStrategy;
class PruneStrategy;
} // namespace probfd::merge_and_shrink

namespace probfd::merge_and_shrink {

/// Runs the merge-and-shrink algorithm on the given factored TS
/// for the given planning task with the given settings.
void run_merge_and_shrink_algorithm(
    FactoredTransitionSystem& fts,
    const SharedProbabilisticTask& task,
    MergeStrategyFactory& merge_strategy,
    ShrinkStrategy& shrink_strategy,
    LabelReduction* label_reduction,
    PruneStrategy& prune_strategy,
    int max_states,
    int max_states_before_merge,
    int shrink_threshold_before_merge,
    downward::utils::FSeconds main_loop_max_time,
    const downward::utils::LogProxy& log);

} // namespace probfd::merge_and_shrink

#endif
