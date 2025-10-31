#include "probfd/cli/merge_and_shrink/merge_and_shrink_algorithm_options.h"

#include "probfd/merge_and_shrink/label_reduction.h"
#include "probfd/merge_and_shrink/labels.h"
#include "probfd/merge_and_shrink/merge_strategy_factory.h"
#include "probfd/merge_and_shrink/prune_strategy.h"
#include "probfd/merge_and_shrink/shrink_strategy.h"

#include "downward/utils/math.h"

#include "downward/cli/plugins/options.h"
#include "downward/cli/plugins/plugin.h"

#include <string>

using namespace std;
using namespace downward::cli::plugins;
using namespace downward;
using namespace probfd::merge_and_shrink;

namespace probfd::cli::merge_and_shrink {

void add_merge_and_shrink_algorithm_options_to_feature(Feature& feature)
{
    // Merge strategy option.
    feature.add_optional_argument<shared_ptr<MergeStrategyFactory>>(
        "merge_strategy",
        "See detailed documentation for merge strategies.");

    // Shrink strategy option.
    feature.add_optional_argument_with_default<shared_ptr<ShrinkStrategy>>(
        "shrink_strategy",
        "pshrink_random()",
        "See detailed documentation for shrink strategies.");

    // Pruning strategy option.
    feature.add_optional_argument_with_default<shared_ptr<PruneStrategy>>(
        "prune_strategy",
        "prune_identity()",
        "See detailed documentation for pruning strategies.");

    // Label reduction option.
    feature.add_optional_argument<shared_ptr<LabelReduction>>(
        "label_reduction",
        "See detailed documentation for labels. There is currently only "
        "one 'option' to use label_reduction, which is "
        "{{{label_reduction=exact()}}} "
        "Also note the interaction with shrink strategies.");

    add_transition_system_size_limit_options_to_feature(feature);

    feature.add_optional_duration_argument_with_default(
        "main_loop_max_time",
        "infinite",
        "A limit in seconds on the runtime of the main loop of the algorithm. "
        "If the limit is exceeded, the algorithm terminates, potentially "
        "returning a factored transition system with several factors. Also "
        "note that the time limit is only checked between transformations "
        "of the main loop, but not during, so it can be exceeded if a "
        "transformation is runtime-intense.");
}

tuple<
    shared_ptr<MergeStrategyFactory>,
    shared_ptr<ShrinkStrategy>,
    shared_ptr<LabelReduction>,
    shared_ptr<PruneStrategy>,
    int,
    int,
    int,
    downward::utils::FSeconds>
get_merge_and_shrink_algorithm_arguments_from_options(const Options& opts)
{
    return tuple_cat(
        make_tuple(
            opts.get<shared_ptr<MergeStrategyFactory>>("merge_strategy"),
            opts.get<shared_ptr<ShrinkStrategy>>("shrink_strategy"),
            opts.get<shared_ptr<LabelReduction>>("label_reduction", nullptr),
            opts.get<shared_ptr<PruneStrategy>>("prune_strategy")),
        get_transition_system_size_limit_arguments_from_options(opts),
        make_tuple(opts.get<downward::utils::FSeconds>("main_loop_max_time")));
}

void add_transition_system_size_limit_options_to_feature(Feature& feature)
{
    feature.add_optional_argument_with_default<int>(
        "max_states",
        "-1",
        "maximum transition system size allowed at any time point.");
    feature.add_optional_argument_with_default<int>(
        "max_states_before_merge",
        "-1",
        "maximum transition system size allowed for two transition systems "
        "before being merged to form the synchronized product.");
    feature.add_optional_argument_with_default<int>(
        "threshold_before_merge",
        "-1",
        "If a transition system, before being merged, surpasses this soft "
        "transition system size limit, the shrink strategy is called to "
        "possibly shrink the transition system.");
}

tuple<int, int, int>
get_transition_system_size_limit_arguments_from_options(const Options& opts)
{
    return make_tuple(
        opts.get<int>("max_states"),
        opts.get<int>("max_states_before_merge"),
        opts.get<int>("threshold_before_merge"));
}

void handle_shrink_limit_options_defaults(
    int& max_states,
    int& max_states_before_merge,
    int& threshold_before_merge,
    const utils::Context& context)
{
    if (max_states < -1) {
        throw std::domain_error("max_states must be >= -1.");
    }

    if (max_states_before_merge < -1) {
        throw std::domain_error("max_states_before_merge must be >= -1.");
    }

    if (threshold_before_merge < -1) {
        throw std::domain_error("threshold_before_merge must be >= -1.");
    }

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

    if (threshold_before_merge == -1) { threshold_before_merge = max_states; }
    if (threshold_before_merge < 1) {
        context.error("Threshold must be at least 1");
    }
    if (threshold_before_merge > max_states) {
        context.warn("warning: threshold exceeds max_states, correcting");
        threshold_before_merge = max_states;
    }
}

} // namespace probfd::cli::merge_and_shrink