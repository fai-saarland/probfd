#include "downward/cli/heuristic_options.h"

#include "downward/cli/evaluator_options.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/utils/logging.h"

#include "downward/tasks/root_task.h"

using namespace std;

namespace downward::cli {

void add_heuristic_options_to_feature(
    plugins::Feature& feature,
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

tuple<
    shared_ptr<AbstractTask>,
    shared_ptr<TaskTransformation>,
    bool,
    string,
    utils::Verbosity>
get_heuristic_arguments_from_options(const plugins::Options& opts)
{
    return tuple_cat(
        make_tuple(
            tasks::g_root_task,
            opts.get<shared_ptr<TaskTransformation>>("transform"),
            opts.get<bool>("cache_estimates")),
        get_evaluator_arguments_from_options(opts));
}

} // namespace downward::cli