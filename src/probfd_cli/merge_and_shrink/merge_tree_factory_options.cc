#include "probfd_cli/merge_and_shrink/merge_tree_factory_options.h"

#include "probfd/merge_and_shrink/merge_tree.h"

#include "downward/utils/logging.h"

#include "downward_cli/utils/rng_options.h"
#include "language/plugins/plugin.h"

using namespace std;
using namespace probfd::merge_and_shrink;

using namespace language;
using namespace language::plugins;

namespace probfd::cli::merge_and_shrink {

void add_merge_tree_factory_options_to_feature(Feature& feature)
{
    downward::cli::utils::add_rng_options_to_feature(feature);

    feature.add_option<UpdateOption>(
        "update_option",
        "When the merge tree is used within another merge strategy, how "
        "should it be updated when a merge different to a merge from the "
        "tree is performed.",
        "use_random");
}

std::tuple<int, UpdateOption> get_merge_tree_factory_args_from_options(
    const Context& context,
    const Options& options)
{
    return std::tuple_cat(
        downward::cli::utils::get_rng_arguments_from_options(context, options),
        std::make_tuple(options.get<UpdateOption>(context, "update_option")));
}

} // namespace probfd::cli::merge_and_shrink