#include "downward/cli/heuristics/heuristic_options.h"

#include "downward/cli/evaluators/evaluator_options.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/utils/logging.h"

#include "downward/tasks/root_task.h"

using namespace std;

namespace downward::cli {

void add_heuristic_options_to_feature(
    plugins::Feature& feature,
    const string& description)
{
    feature.add_optional_argument_with_default<shared_ptr<TaskTransformation>>(
        "transform",
        "no_transform()",
        "Optional task transformation for the heuristic.");
    feature.add_optional_argument_with_default<bool>(
        "cache_estimates",
        "true",
        "cache heuristic estimates");
    add_evaluator_options_to_feature(feature, description);
}

tuple<shared_ptr<TaskTransformation>, bool, string, utils::Verbosity>
get_heuristic_arguments_from_options(const plugins::Options& opts)
{
    return tuple_cat(
        make_tuple(
            opts.get<shared_ptr<TaskTransformation>>("transform"),
            opts.get<bool>("cache_estimates")),
        get_evaluator_arguments_from_options(opts));
}

} // namespace downward::cli