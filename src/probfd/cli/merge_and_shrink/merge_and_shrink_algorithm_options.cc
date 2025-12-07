#include "probfd/cli/merge_and_shrink/merge_and_shrink_algorithm_options.h"

#include "probfd/merge_and_shrink/label_reduction.h"

#include "downward/utils/math.h"

#include "language/plugins/options.h"
#include "language/plugins/plugin.h"

#include <string>

using namespace std;
using namespace downward::cli::plugins;
using namespace downward;
using namespace probfd::merge_and_shrink;

namespace probfd::cli::merge_and_shrink {

std::size_t add_transition_system_size_limit_options_to_feature(
    InternalFunctionDefinitionBase& feature,
    std::size_t start_index)
{
    feature.make_optional_argument_with_default(
        start_index,
        "max_states",
        "-1",
        "maximum transition system size allowed at any time point.");

    feature.make_optional_argument_with_default(
        start_index + 1,
        "max_states_before_merge",
        "-1",
        "maximum transition system size allowed for two transition systems "
        "before being merged to form the synchronized product.");

    feature.make_optional_argument_with_default(
        start_index + 2,
        "threshold_before_merge",
        "-1",
        "If a transition system, before being merged, surpasses this soft "
        "transition system size limit, the shrink strategy is called to "
        "possibly shrink the transition system.");

    return 3;
}

void handle_shrink_limit_options_defaults(
    int& max_states,
    int& max_states_before_merge,
    int& threshold_before_merge)
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
        max_states_before_merge = max_states;
    }

    if (max_states < 1) {
        throw std::domain_error("Transition system size must be at least 1");
    }

    if (max_states_before_merge < 1) {
        throw std::domain_error(
            "Transition system size before merge must be at least 1");
    }

    if (threshold_before_merge == -1) { threshold_before_merge = max_states; }

    if (threshold_before_merge < 1) {
        throw std::domain_error("Threshold must be at least 1");
    }

    if (threshold_before_merge > max_states) {
        threshold_before_merge = max_states;
    }
}

} // namespace probfd::cli::merge_and_shrink