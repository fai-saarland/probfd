#include "downward/cli/evaluators/combining_evaluator_options.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/cli/evaluator_options.h"

#include "downward/utils/logging.h"

#include "downward/evaluator.h"

using namespace std;

using namespace downward::cli::plugins;

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

tuple<vector<shared_ptr<Evaluator>>, const string, utils::Verbosity>
get_combining_evaluator_arguments_from_options(const Options& opts)
{
    return tuple_cat(
        make_tuple(opts.get_list<shared_ptr<Evaluator>>("evals")),
        get_evaluator_arguments_from_options(opts));
}

} // namespace downward::cli::combining_evaluator
