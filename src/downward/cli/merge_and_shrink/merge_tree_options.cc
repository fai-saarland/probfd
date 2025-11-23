#include "downward/cli/merge_and_shrink/merge_tree_options.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/cli/utils/rng_options.h"

using namespace std;

using namespace downward::cli::utils;

namespace downward::cli::merge_and_shrink {

std::size_t add_merge_tree_options_to_feature(
    plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index)
{
    const auto n = add_rng_options_to_feature(feature, start_index);
    feature.make_optional_argument_with_default(
        start_index + n,
        "update_option",
        "use_random",
        "When the merge tree is used within another merge strategy, how "
        "should it be updated when a merge different to a merge from the "
        "tree is performed.");

    return n + 1;
}

} // namespace downward::cli::merge_and_shrink
