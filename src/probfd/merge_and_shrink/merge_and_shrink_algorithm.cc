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

#include "probfd/task_proxy.h"

#include "probfd/utils/guards.h"

#include "downward/task_utils/task_properties.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/math.h"
#include "downward/utils/system.h"
#include "downward/utils/timer.h"

#include "downward/cli/plugins/options.h"
#include "downward/cli/plugins/plugin.h"

#include <cassert>
#include <iostream>
#include <string>
#include <utility>

using utils::ExitCode;

using namespace std;
using namespace downward::cli::plugins;

namespace probfd::merge_and_shrink {

static void
log_progress(const utils::Timer& timer, const string& msg, utils::LogProxy& log)
{
    log << "M&S algorithm timer: " << timer << " (" << msg << ")" << endl;
}

MergeAndShrinkAlgorithm::MergeAndShrinkAlgorithm(
    std::shared_ptr<MergeStrategyFactory> merge_strategy,
    std::shared_ptr<ShrinkStrategy> shrink_strategy,
    std::shared_ptr<LabelReduction> label_reduction,
    std::shared_ptr<PruneStrategy> prune_strategy,
    int max_states,
    int max_states_before_merge,
    int threshold_before_merge,
    bool prune_unreachable_states,
    bool prune_irrelevant_states,
    double main_loop_max_time)
    : merge_strategy_factory(std::move(merge_strategy))
    , shrink_strategy(std::move(shrink_strategy))
    , label_reduction(std::move(label_reduction))
    , prune_strategy(std::move(prune_strategy))
    , max_states(max_states)
    , max_states_before_merge(max_states_before_merge)
    , shrink_threshold_before_merge(threshold_before_merge)
    , prune_unreachable_states(prune_unreachable_states)
    , prune_irrelevant_states(prune_irrelevant_states)
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
        log << "Final";
    else
        log << "Current";
    log << " peak memory increase of merge-and-shrink algorithm: "
        << utils::get_peak_memory_in_kb() - starting_peak_memory << " KB"
        << endl;
}

void MergeAndShrinkAlgorithm::dump_options(utils::LogProxy log) const
{
    if (log.is_at_least_normal()) {
        if (merge_strategy_factory) { // deleted after merge strategy extraction
            merge_strategy_factory->dump_options();
            log << endl;
        }

        log << "Options related to size limits and shrinking: " << endl;
        log << "Transition system size limit: " << max_states << endl
            << "Transition system size limit right before merge: "
            << max_states_before_merge << endl;
        log << "Threshold to trigger shrinking right before merge: "
            << shrink_threshold_before_merge << endl;
        log << endl;

        if (prune_strategy) {
            prune_strategy->dump_options(log);
        } else {
            log << "Pruning disabled" << endl;
        }

        log << endl;

        if (label_reduction) {
            label_reduction->dump_options(log);
        } else {
            log << "Label reduction disabled" << endl;
        }
        log << endl;

        log << "Main loop max time in seconds: " << main_loop_max_time << endl;
        log << endl;
    }
}

void MergeAndShrinkAlgorithm::warn_on_unusual_options(utils::LogProxy log) const
{
    const string dashes(79, '=');
    if (!label_reduction) {
        if (log.is_warning()) {
            log << dashes << endl
                << "WARNING! You did not enable label reduction. " << endl
                << "This may drastically reduce the performance of "
                   "merge-and-shrink!"
                << endl
                << dashes << endl;
        }
    } else if (
        label_reduction->reduce_before_merging() &&
        label_reduction->reduce_before_shrinking()) {
        if (log.is_warning()) {
            log << dashes << endl
                << "WARNING! You set label reduction to be applied twice in "
                   "each merge-and-shrink"
                << endl
                << "iteration, both before shrinking and merging. This double "
                   "computation effort"
                << endl
                << "does not pay off for most configurations!" << endl
                << dashes << endl;
        }
    } else {
        if (label_reduction->reduce_before_shrinking() &&
            (shrink_strategy->get_name() == "f-preserving" ||
             shrink_strategy->get_name() == "random")) {
            if (log.is_warning()) {
                log << dashes << endl
                    << "WARNING! Bucket-based shrink strategies such as "
                       "f-preserving random perform"
                    << endl
                    << "best if used with label reduction before merging, not "
                       "before shrinking!"
                    << endl
                    << dashes << endl;
            }
        }
        if (label_reduction->reduce_before_merging() &&
            shrink_strategy->get_name() == "bisimulation") {
            if (log.is_warning()) {
                log << dashes << endl
                    << "WARNING! Shrinking based on bisimulation performs best "
                       "if used with label"
                    << endl
                    << "reduction before shrinking, not before merging!" << endl
                    << dashes << endl;
            }
        }
    }

    if (!prune_strategy) {
        if (log.is_warning()) {
            log << dashes << endl
                << "WARNING! Pruning is turned off!" << endl
                << "This may drastically reduce the performance of "
                   "merge-and-shrink!"
                << endl
                << dashes << endl;
        }
    }
}

void MergeAndShrinkAlgorithm::main_loop(
    FactoredTransitionSystem& fts,
    MergeStrategy& merge_strategy,
    const utils::CountdownTimer& timer,
    utils::LogProxy log)
{
    if (log.is_at_least_normal()) {
        log << "Starting main loop ";
        if (main_loop_max_time == numeric_limits<double>::infinity()) {
            log << "without a time limit." << endl;
        } else {
            log << "with a time limit of " << main_loop_max_time << "s."
                << endl;
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
        log << "M&S algorithm main loop timer: " << timer.get_elapsed_time()
            << " (" << msg << ")" << endl;
    };

    auto ran_out_of_time = [&](const auto& t) {
        if (t.is_expired()) {
            if (log.is_at_least_normal()) {
                log << "Ran out of time, stopping computation." << endl;
                log << endl;
            }
            return true;
        }
        return false;
    };

    while (fts.get_num_active_entries() > 1) {
        // Choose next transition systems to merge
        const auto [merge_index1, merge_index2] = merge_strategy.get_next();

        if (ran_out_of_time(timer)) {
            break;
        }

        assert(merge_index1 != merge_index2);
        if (log.is_at_least_normal()) {
            log << "Next pair of indices: (" << merge_index1 << ", "
                << merge_index2 << ")" << endl;
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

        if (ran_out_of_time(timer)) {
            break;
        }

        // Shrinking
        const bool shrunk = shrink_before_merge_step(
            fts,
            merge_index1,
            merge_index2,
            max_states,
            max_states_before_merge,
            shrink_threshold_before_merge,
            *shrink_strategy,
            log);
        if (log.is_at_least_normal() && shrunk) {
            log_main_loop_progress("after shrinking");
        }

        if (ran_out_of_time(timer)) {
            break;
        }

        // Label reduction (before merging)
        if (label_reduction && label_reduction->reduce_before_merging()) {
            const bool reduced =
                label_reduction->reduce(merge_index1, merge_index2, fts, log);
            if (log.is_at_least_normal() && reduced) {
                log_main_loop_progress("after label reduction");
            }
        }

        if (ran_out_of_time(timer)) {
            break;
        }

        // Merging
        const int merged_index = fts.merge(merge_index1, merge_index2, log);
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

        if (ran_out_of_time(timer)) {
            break;
        }

        // Pruning
        if (prune_strategy) {
            auto pruning_relation = prune_strategy->compute_pruning_abstraction(
                fts.get_transition_system(merged_index),
                fts.get_distances(merged_index),
                log);

            const bool pruned =
                fts.apply_abstraction(merged_index, pruning_relation, log);
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
                log << "Abstract problem is unsolvable, stopping "
                       "computation. "
                    << endl
                    << endl;
            }
            break;
        }

        if (ran_out_of_time(timer)) {
            break;
        }

        // End-of-iteration output.
        if (log.is_at_least_verbose()) {
            report_peak_memory_delta(log);
        }
        if (log.is_at_least_normal()) {
            log << endl;
        }
    }

    log << "End of merge-and-shrink algorithm, statistics:" << endl;
    log << "Main loop runtime: " << timer.get_elapsed_time() << endl;
    log << "Maximum intermediate abstraction size: "
        << maximum_intermediate_size << endl;
}

FactoredTransitionSystem
MergeAndShrinkAlgorithm::build_factored_transition_system(
    const ProbabilisticTaskProxy& task_proxy,
    utils::LogProxy log)
{
    if (starting_peak_memory) {
        cerr << "Calling build_factored_transition_system twice is not "
             << "supported!" << endl;
        exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }
    starting_peak_memory = utils::get_peak_memory_in_kb();

    const utils::Timer timer;
    log << "Running merge-and-shrink algorithm..." << endl;
    ::task_properties::verify_no_axioms(task_proxy);
    task_properties::verify_no_conditional_effects(task_proxy);
    dump_options(log);
    warn_on_unusual_options(log);
    log << endl;

    const bool compute_init_distances =
        shrink_strategy->requires_init_distances() ||
        merge_strategy_factory->requires_init_distances() ||
        prune_strategy->requires_init_distances();
    const bool compute_goal_distances =
        shrink_strategy->requires_goal_distances() ||
        merge_strategy_factory->requires_goal_distances() ||
        prune_strategy->requires_goal_distances();
    FactoredTransitionSystem fts = create_factored_transition_system(
        task_proxy,
        compute_init_distances,
        compute_goal_distances,
        log);
    if (log.is_at_least_normal()) {
        log_progress(timer, "after computation of atomic factors", log);
    }

    scope_exit scope([&] {
        report_peak_memory_delta(log, true);
        log << "Merge-and-shrink algorithm runtime: " << timer << endl;
        log << endl;
    });

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
                log << endl;
            }
        });

        for (int index = 0; index < fts.get_size(); ++index) {
            assert(fts.is_active(index));
            if (prune_strategy) {
                auto pruning_relation =
                    prune_strategy->compute_pruning_abstraction(
                        fts.get_transition_system(index),
                        fts.get_distances(index),
                        log);
                const bool pruned_factor =
                    fts.apply_abstraction(index, pruning_relation, log);
                pruned = pruned || pruned_factor;
            }
            if (!fts.is_factor_solvable(index)) {
                log << "Atomic FTS is unsolvable, stopping computation."
                    << endl;
                return fts;
            }
        }
    }

    if (main_loop_max_time <= 0) return fts;

    const utils::CountdownTimer loop_timer(main_loop_max_time);
    if (log.is_at_least_normal()) {
        log << "Starting main loop ";
        if (main_loop_max_time == numeric_limits<double>::infinity()) {
            log << "without a time limit." << endl;
        } else {
            log << "with a time limit of " << main_loop_max_time << "s."
                << endl;
        }
    }

    if (label_reduction) {
        label_reduction->initialize(task_proxy);
    }

    const unique_ptr<MergeStrategy> merge_strategy =
        merge_strategy_factory->compute_merge_strategy(task_proxy, fts);
    merge_strategy_factory = nullptr;

    main_loop(fts, *merge_strategy, loop_timer, log);

    shrink_strategy = nullptr;
    label_reduction = nullptr;

    return fts;
}

void add_merge_and_shrink_algorithm_options_to_feature(Feature& feature)
{
    // Merge strategy option.
    feature.add_option<shared_ptr<MergeStrategyFactory>>(
        "merge_strategy",
        "See detailed documentation for merge strategies. "
        "We currently recommend SCC-DFP, which can be achieved using "
        "{{{merge_strategy=merge_sccs(order_of_sccs=topological,merge_selector="
        "score_based_filtering(scoring_functions=[goal_relevance,dfp,total_"
        "order"
        "]))}}}");

    // Shrink strategy option.
    feature.add_option<shared_ptr<ShrinkStrategy>>(
        "shrink_strategy",
        "See detailed documentation for shrink strategies. "
        "We currently recommend non-greedy shrink_bisimulation, which can be "
        "achieved using "
        "{{{shrink_strategy=shrink_bisimulation(greedy=false)}}}");

    // Pruning strategy option.
    feature.add_option<shared_ptr<PruneStrategy>>(
        "pruning_strategy",
        "See detailed documentation for pruning strategies.",
        ArgumentInfo::NO_DEFAULT);

    // Label reduction option.
    feature.add_option<shared_ptr<LabelReduction>>(
        "label_reduction",
        "See detailed documentation for labels. There is currently only "
        "one 'option' to use label_reduction, which is "
        "{{{label_reduction=exact}}} "
        "Also note the interaction with shrink strategies.",
        ArgumentInfo::NO_DEFAULT);

    add_transition_system_size_limit_options_to_feature(feature);

    feature.add_option<double>(
        "main_loop_max_time",
        "A limit in seconds on the runtime of the main loop of the algorithm. "
        "If the limit is exceeded, the algorithm terminates, potentially "
        "returning a factored transition system with several factors. Also "
        "note that the time limit is only checked between transformations "
        "of the main loop, but not during, so it can be exceeded if a "
        "transformation is runtime-intense.",
        "infinity",
        Bounds("0.0", "infinity"));
}

void add_transition_system_size_limit_options_to_feature(Feature& feature)
{
    feature.add_option<int>(
        "max_states",
        "maximum transition system size allowed at any time point.",
        "-1",
        Bounds("-1", "infinity"));
    feature.add_option<int>(
        "max_states_before_merge",
        "maximum transition system size allowed for two transition systems "
        "before being merged to form the synchronized product.",
        "-1",
        Bounds("-1", "infinity"));
    feature.add_option<int>(
        "threshold_before_merge",
        "If a transition system, before being merged, surpasses this soft "
        "transition system size limit, the shrink strategy is called to "
        "possibly shrink the transition system.",
        "-1",
        Bounds("-1", "infinity"));
}

void handle_shrink_limit_options_defaults(
    Options& opts,
    const utils::Context& context)
{
    int max_states = opts.get<int>("max_states");
    int max_states_before_merge = opts.get<int>("max_states_before_merge");
    int threshold = opts.get<int>("threshold_before_merge");

    // If none of the two state limits has been set: set default limit.
    if (max_states == -1 && max_states_before_merge == -1) {
        max_states = 50000;
    }

    // If exactly one of the max_states options has been set, set the other
    // so that it imposes no further limits.
    if (max_states_before_merge == -1) {
        max_states_before_merge = max_states;
    } else if (max_states == -1) {
        if (utils::is_product_within_limit(
                max_states_before_merge,
                max_states_before_merge,
                std::numeric_limits<int>::max())) {
            max_states = max_states_before_merge * max_states_before_merge;
        } else {
            max_states = std::numeric_limits<int>::max();
        }
    }

    if (max_states_before_merge > max_states) {
        context.warn(
            "warning: max_states_before_merge exceeds max_states, correcting.");
        max_states_before_merge = max_states;
    }

    if (max_states < 1) {
        context.error("Transition system size must be at least 1");
    }

    if (max_states_before_merge < 1) {
        context.error("Transition system size before merge must be at least 1");
    }

    if (threshold == -1) {
        threshold = max_states;
    }
    if (threshold < 1) {
        context.error("Threshold must be at least 1");
    }
    if (threshold > max_states) {
        context.warn("warning: threshold exceeds max_states, correcting");
        threshold = max_states;
    }

    opts.set<int>("max_states", max_states);
    opts.set<int>("max_states_before_merge", max_states_before_merge);
    opts.set<int>("threshold_before_merge", threshold);
}

} // namespace probfd::merge_and_shrink