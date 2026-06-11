#include "downward_cli/heuristics/heuristic_options.h"

#include "downward_cli/evaluators/evaluator_options.h"

#include "language/plugins/plugin.h"

#include "downward/utils/logging.h"

using namespace std;

using namespace language;
using namespace language::plugins;

namespace downward::cli {

void add_heuristic_options_to_feature(
    Feature& feature,
    const string& description)
{
    feature.add_option<shared_ptr<TaskTransformation>>(
        "transform",
        "Optional task transformation for the heuristic."
        " Currently, adapt_costs() and no_transform() are available.",
        "no_transform()");
    feature.add_option<bool>(
        "cache_estimates",
        "cache heuristic estimates",
        "true");
    add_evaluator_options_to_feature(feature, description);
}

tuple<shared_ptr<TaskTransformation>, bool, string, utils::Verbosity>
get_heuristic_arguments_from_options(
    const Context& context,
    const Options& opts)
{
    return tuple_cat(
        make_tuple(
            opts.get<shared_ptr<TaskTransformation>>(context, "transform"),
            opts.get<bool>(context, "cache_estimates")),
        get_evaluator_arguments_from_options(context, opts));
}

} // namespace downward::cli