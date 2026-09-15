#include "downward_cli/merge_and_shrink/merge_tree_options.h"

#include "language/plugins/plugin.h"

#include "downward_cli/utils/rng_options.h"

using namespace std;
using namespace downward::merge_and_shrink;

using namespace language;

using namespace downward::cli::utils;

namespace downward::cli::merge_and_shrink {

void add_merge_tree_options_to_feature(plugins::Feature& feature)
{
    feature.add_option<std::shared_ptr<MergeUpdateStrategy>>(
        "tree_update_strategy",
        "When the merge tree is used within another merge strategy, how "
        "should it be updated when a merge different to a merge from the "
        "tree is performed.",
        "use_random");
}

tuple<std::shared_ptr<MergeUpdateStrategy>>
get_merge_tree_arguments_from_options(
    const Context& context,
    const plugins::Options& opts)
{
    return std::make_tuple(opts.get<std::shared_ptr<MergeUpdateStrategy>>(
        context,
        "tree_update_strategy"));
}

} // namespace downward::cli::merge_and_shrink
