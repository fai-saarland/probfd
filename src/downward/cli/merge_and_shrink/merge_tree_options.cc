#include "downward/cli/merge_and_shrink/merge_tree_options.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/cli/utils/rng_options.h"

using namespace std;
using namespace downward::merge_and_shrink;

using namespace downward::cli::utils;

namespace downward::cli::merge_and_shrink {

void add_merge_tree_options_to_feature(plugins::Feature& feature)
{
    add_rng_options_to_feature(feature);
    feature.add_option<UpdateOption>(
        "update_option",
        "When the merge tree is used within another merge strategy, how "
        "should it be updated when a merge different to a merge from the "
        "tree is performed.",
        "use_random");
}

tuple<int, UpdateOption>
get_merge_tree_arguments_from_options(const plugins::Options& opts)
{
    return tuple_cat(
        get_rng_arguments_from_options(opts),
        make_tuple(opts.get<UpdateOption>("update_option")));
}

} // namespace downward::cli::merge_and_shrink
