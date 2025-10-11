#include "probfd/merge_and_shrink/merge_and_shrink_algorithm.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/fts_factory.h"
#include "probfd/merge_and_shrink/label_reduction.h"
#include "probfd/merge_and_shrink/labels.h"
#include "probfd/merge_and_shrink/merge_strategy.h"
#include "probfd/merge_and_shrink/merge_strategy_factory.h"
#include "probfd/merge_and_shrink/prune_strategy.h"
#include "probfd/merge_and_shrink/shrink_strategy.h"
#include "probfd/merge_and_shrink/transition_system.h"
#include "probfd/merge_and_shrink/utils.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/utils/guards.h"

#include "downward/task_utils/task_properties.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/system.h"
#include "downward/utils/timer.h"

#include "downward/cli/plugins/plugin.h"
#include "probfd/merge_and_shrink/factored_mapping.h"

#include <cassert>
#include <iostream>
#include <string>

using namespace std;
using namespace downward;
using utils::ExitCode;

namespace probfd::merge_and_shrink {

static void
log_progress(const utils::Timer& timer, const string& msg, utils::LogProxy& log)
{
    log.println("M&S algorithm timer: {} ({})", timer(), msg);
}

MergeAndShrinkAlgorithm::MergeAndShrinkAlgorithm(
    std::shared_ptr<MergeStrategyFactory> merge_strategy,
    std::shared_ptr<ShrinkStrategy> shrink_strategy,
    std::shared_ptr<LabelReduction> label_reduction,
    std::shared_ptr<PruneStrategy> prune_strategy,
    int max_states,
    int max_states_before_merge,
    int threshold_before_merge,
    downward::utils::Duration main_loop_max_time)
    : merge_strategy_factory(std::move(merge_strategy))
    , shrink_strategy(std::move(shrink_strategy))
    , label_reduction(std::move(label_reduction))
    , prune_strategy(std::move(prune_strategy))
    , max_states(max_states)
    , max_states_before_merge(max_states_before_merge)
    , shrink_threshold_before_merge(threshold_before_merge)
    , main_loop_max_time(main_loop_max_time)
    , starting_peak_memory(0)
{
    assert(max_states_before_merge > 0);
    assert(max_states >= max_states_before_merge);
    assert(shrink_threshold_before_merge <= max_states_before_merge);
}

void MergeAndShrinkAlgorithm::report_peak_memory_delta(
    utils::LogProxy log,
    bool final) const
{
    if (final)
        log.print("Final");
    else
        log.print("Current");
    log.println(
        " peak memory increase of merge-and-shrink algorithm: {}",
        utils::get_peak_memory_in_kb() - starting_peak_memory);
}

void MergeAndShrinkAlgorithm::dump_options(utils::LogProxy log) const
{
    if (log.is_at_least_normal()) {
        if (merge_strategy_factory) { // deleted after merge strategy extraction
            merge_strategy_factory->dump_options();
            log.println();
        }

        log.println("Options related to size limits and shrinking: {}");
        log.println("Transition system size limit: {}", max_states);
        log.println(
            "Transition system size limit right before merge: {}",
            max_states_before_merge);
        log.println(
            "Threshold to trigger shrinking right before merge: {}",
            shrink_threshold_before_merge);
        log.println();

        if (prune_strategy) {
            prune_strategy->dump_options(log);
        } else {
            log.println("Pruning disabled");
        }

        log.println();

        if (label_reduction) {
            label_reduction->dump_options(log);
        } else {
            log.println("Label reduction disabled");
        }
        log.println();

        log.println("Main loop max time in seconds: {}", main_loop_max_time);
        log.println();
    }
}

void MergeAndShrinkAlgorithm::warn_on_unusual_options(utils::LogProxy log) const
{
    const string dashes(79, '=');
    if (!label_reduction) {
        if (log.is_warning()) {
            log.println(
                "{0}\n"
                "WARNING! You did not enable label reduction. "
                "This may drastically reduce the performance of "
                "merge-and-shrink!\n"
                "{0}",
                dashes);
        }
    } else if (
        label_reduction->reduce_before_merging() &&
        label_reduction->reduce_before_shrinking()) {
        if (log.is_warning()) {
            log.println(
                "{0}\n"
                "WARNING! You set label reduction to be applied twice in each "
                "merge-and-shrink iteration, both before shrinking and "
                "merging. This double computation effort does not pay off for "
                "most configurations!\n"
                "{0}",
                dashes);
        }
    }

    if (!prune_strategy) {
        if (log.is_warning()) {
            log.println(
                "{0}\n"
                "WARNING! Pruning is turned off! This may drastically reduce "
                "the performance of merge-and-shrink!\n"
                "{0}",
                dashes);
        }
    }
}

void MergeAndShrinkAlgorithm::main_loop(
    FactoredTransitionSystem& fts,
    MergeStrategy& merge_strategy,
    bool compute_liveness,
    bool compute_goal_distances,
    const utils::CountdownTimer& timer,
    utils::LogProxy log)
{
    if (log.is_at_least_normal()) {
        log.print("Starting main loop ");
        if (main_loop_max_time == utils::Duration::max()) {
            log.println("without a time limit.");
        } else {
            log.println("with a time limit of {}.", main_loop_max_time);
        }
    }

    int maximum_intermediate_size = 0;
    for (int i = 0; i < fts.get_size(); ++i) {
        if (const int size = fts.get_transition_system(i).get_size();
            size > maximum_intermediate_size) {
            maximum_intermediate_size = size;
        }
    }

    auto log_main_loop_progress = [&](const string& msg) {
        log.println(
            "M&S algorithm main loop timer: {} ({})",
            timer.get_elapsed_time(),
            msg);
    };

    auto ran_out_of_time = [&](const auto& t) {
        if (t.is_expired()) {
            if (log.is_at_least_normal()) {
                log.println("Ran out of time, stopping computation.");
                log.println();
            }
            return true;
        }
        return false;
    };

    while (fts.get_num_active_entries() > 1) {
        // Choose next transition systems to merge
        const auto index_pair = merge_strategy.get_next();
        const auto [merge_index1, merge_index2] = index_pair;

        if (ran_out_of_time(timer)) { break; }

        assert(merge_index1 != merge_index2);
        if (log.is_at_least_normal()) {
            log.println("Next pair of indices: {}", index_pair);
            if (log.is_at_least_verbose()) {
                fts.statistics(merge_index1, log);
                fts.statistics(merge_index2, log);
            }
            log_main_loop_progress("after computation of next merge");
        }

        // Label reduction (before shrinking)
        if (label_reduction && label_reduction->reduce_before_shrinking()) {
            const bool reduced =
                label_reduction->reduce(merge_index1, merge_index2, fts, log);
            if (log.is_at_least_normal() && reduced) {
                log_main_loop_progress("after label reduction");
            }
        }

        if (ran_out_of_time(timer)) { break; }

        // Shrinking
        const bool shrunk = shrink_before_merge_step(
            fts,
            merge_index1,
            merge_index2,
            max_states,
            max_states_before_merge,
            shrink_threshold_before_merge,
            *shrink_strategy,
            compute_goal_distances,
            compute_liveness,
            log);
        if (log.is_at_least_normal() && shrunk) {
            log_main_loop_progress("after shrinking");
        }

        if (ran_out_of_time(timer)) { break; }

        // Label reduction (before merging)
        if (label_reduction && label_reduction->reduce_before_merging()) {
            const bool reduced =
                label_reduction->reduce(merge_index1, merge_index2, fts, log);
            if (log.is_at_least_normal() && reduced) {
                log_main_loop_progress("after label reduction");
            }
        }

        if (ran_out_of_time(timer)) { break; }

        // Merging
        auto&& [left_factor, right_factor, factor, merged_index] =
            fts.merge(merge_index1, merge_index2, log);

        class MergeHeuristic : public Heuristic<int> {
            const FactoredMappingMerge& merge_fm;
            std::vector<value_t> distance_table1;
            std::vector<value_t> distance_table2;

        public:
            MergeHeuristic(
                const FactoredMappingMerge& merge_fm,
                Distances& distances1,
                Distances& distances2)
                : merge_fm(merge_fm)
                , distance_table1(distances1.extract_goal_distances())
                , distance_table2(distances2.extract_goal_distances())
            {
            }

            value_t evaluate(int state) const override
            {
                const auto [left, right] = merge_fm.get_children_states(state);
                return std::max(distance_table1[left], distance_table2[right]);
            }
        };

        // Restore the invariant that distances are computed.
        if (compute_goal_distances) {
            const MergeHeuristic heuristic(
                static_cast<const FactoredMappingMerge&>(
                    *factor.factored_mapping),
                *left_factor.distances,
                *right_factor.distances);

            factor.distances->compute_distances(
                fts.get_labels(),
                *factor.transition_system,
                compute_liveness,
                log,
                heuristic);
        }

        if (const int abs_size =
                fts.get_transition_system(merged_index).get_size();
            abs_size > maximum_intermediate_size) {
            maximum_intermediate_size = abs_size;
        }

        if (log.is_at_least_normal()) {
            if (log.is_at_least_verbose()) {
                fts.statistics(merged_index, log);
            }
            log_main_loop_progress("after merging");
        }

        if (ran_out_of_time(timer)) { break; }

        // Pruning
        if (prune_strategy) {
            auto pruning_relation = prune_strategy->compute_pruning_abstraction(
                fts.get_transition_system(merged_index),
                fts.get_distances(merged_index),
                log);

            const bool pruned = fts.apply_abstraction(
                merged_index,
                pruning_relation,
                compute_goal_distances,
                compute_liveness,
                log);

            if (log.is_at_least_normal() && pruned) {
                if (log.is_at_least_verbose()) {
                    fts.statistics(merged_index, log);
                }
                log_main_loop_progress("after pruning");
            }
        }

        /*
          NOTE: both the shrink strategy classes and the construction
          of the composite transition system require the input
          transition systems to be non-empty, i.e. the initial state
          not to be pruned/not to be evaluated as infinity.
        */
        if (!fts.is_factor_solvable(merged_index)) {
            if (log.is_at_least_normal()) {
                log.println(
                    "Abstract problem is unsolvable, stopping computation.");
                log.println();
            }
            break;
        }

        if (ran_out_of_time(timer)) { break; }

        // End-of-iteration output.
        if (log.is_at_least_verbose()) { report_peak_memory_delta(log); }
        if (log.is_at_least_normal()) { log.println(); }
    }

    log.println("End of merge-and-shrink algorithm, statistics:");
    log.println("Main loop runtime: {}", timer.get_elapsed_time());
    log.println(
        "Maximum intermediate abstraction size: {}",
        maximum_intermediate_size);
}

FactoredTransitionSystem
MergeAndShrinkAlgorithm::build_factored_transition_system(
    const SharedProbabilisticTask& task,
    utils::LogProxy log)
{
    const auto& axioms = get_axioms(task);
    const auto& operators = get_operators(task);

    ::task_properties::verify_no_axioms(axioms);
    task_properties::verify_no_conditional_effects(operators);

    if (starting_peak_memory.count() > 0) {
        throw utils::CriticalError(
            "Calling build_factored_transition_system twice is not supported!");
    }

    starting_peak_memory = utils::get_peak_memory_in_kb();

    const utils::Timer timer;
    log.println("Running merge-and-shrink algorithm...");

    dump_options(log);
    warn_on_unusual_options(log);
    log.println();

    FactoredTransitionSystem fts =
        create_factored_transition_system(to_refs(task), log);

    if (log.is_at_least_normal()) {
        log_progress(timer, "after computation of atomic factors", log);
    }

    scope_exit scope([&] {
        report_peak_memory_delta(log, true);
        log.println("Merge-and-shrink algorithm runtime: {}", timer());
        log.println();
    });

    const bool compute_liveness = shrink_strategy->requires_liveness() ||
                                  merge_strategy_factory->requires_liveness() ||
                                  prune_strategy->requires_liveness();

    const bool compute_goal_distances =
        shrink_strategy->requires_goal_distances() ||
        merge_strategy_factory->requires_goal_distances() ||
        prune_strategy->requires_goal_distances();

    // Restore the invariant that distances are computed.
    if (compute_goal_distances) {
        for (int index : fts) {
            fts.get_distances(index).compute_distances(
                fts.get_labels(),
                fts.get_transition_system(index),
                compute_liveness,
                log,
                heuristics::BlindHeuristic<int>(
                    get_operators(task),
                    get_cost_function(task),
                    get_termination_costs(task)));
        }
    }

    /*
      Prune all atomic factors according to the chosen options. Stop early if
      one factor is unsolvable.

      TODO: think about if we can prune already while creating the atomic FTS.
    */
    {
        bool pruned = false;

        scope_exit _([&] {
            if (log.is_at_least_normal()) {
                if (pruned) {
                    log_progress(timer, "after pruning atomic factors", log);
                }
                log.println();
            }
        });

        for (int index = 0; index < fts.get_size(); ++index) {
            assert(fts.is_active(index));

            auto& ts = fts.get_transition_system(index);
            auto& distances = fts.get_distances(index);

            auto pruning_relation =
                prune_strategy->compute_pruning_abstraction(ts, distances, log);
            const bool pruned_factor = fts.apply_abstraction(
                index,
                pruning_relation,
                compute_goal_distances,
                compute_liveness,
                log);

            pruned = pruned || pruned_factor;

            if (!fts.is_factor_solvable(index)) {
                log.println("Atomic FTS is unsolvable, stopping computation.");
                return fts;
            }
        }
    }

    if (main_loop_max_time == utils::Duration::zero()) return fts;

    const utils::CountdownTimer loop_timer(main_loop_max_time);
    if (log.is_at_least_normal()) {
        log.print("Starting main loop ");
        if (main_loop_max_time == utils::Duration::max()) {
            log.println("without a time limit.");
        } else {
            log.println("with a time limit of {}s.", main_loop_max_time);
        }
    }

    if (label_reduction) { label_reduction->initialize(to_refs(task)); }

    const unique_ptr<MergeStrategy> merge_strategy =
        merge_strategy_factory->compute_merge_strategy(task, fts);
    merge_strategy_factory = nullptr;

    main_loop(
        fts,
        *merge_strategy,
        compute_liveness,
        compute_goal_distances,
        loop_timer,
        log);

    shrink_strategy = nullptr;
    label_reduction = nullptr;

    return fts;
}

} // namespace probfd::merge_and_shrink