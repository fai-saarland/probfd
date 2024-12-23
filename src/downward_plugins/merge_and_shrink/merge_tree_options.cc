#include "downward_plugins/merge_and_shrink/merge_tree_options.h"

#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/utils/rng_options.h"

using namespace std;
using namespace merge_and_shrink;

using namespace downward_plugins::utils;

namespace downward_plugins::merge_and_shrink {

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

} // namespace downward_plugins::merge_and_shrink
