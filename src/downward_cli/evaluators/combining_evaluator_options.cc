#include "downward_cli/evaluators/combining_evaluator_options.h"

#include "language/plugins/plugin.h"

#include "downward_cli/evaluators/evaluator_options.h"

#include "downward/utils/logging.h"

#include "downward/evaluator.h"

using namespace std;

using namespace language::plugins;

namespace downward::cli::combining_evaluator {

void add_combining_evaluator_options_to_feature(
    Feature& feature,
    const string& description)
{
    feature.add_list_option<shared_ptr<Evaluator>>(
        "evals",
        "at least one evaluator");
    add_evaluator_options_to_feature(feature, description);
}

tuple<
    const string,
    utils::Verbosity,
    vector<shared_ptr<TaskDependentFactory<Evaluator>>>>
get_combining_evaluator_arguments_from_options(
    const language::Context& context,
    const Options& opts)
{
    return tuple_cat(
        get_evaluator_arguments_from_options(context, opts),
        make_tuple(opts.get_list<shared_ptr<TaskDependentFactory<Evaluator>>>(
            context,
            "evals")));
}

} // namespace downward::cli::combining_evaluator
