#include "downward_cli/evaluators/evaluator_options.h"

#include "downward_cli/utils/logging_options.h"

#include "language/plugins/plugin.h"

#include "downward/utils/logging.h"

using namespace std;

using namespace language;
using namespace language::plugins;

namespace downward::cli {

void add_evaluator_options_to_feature(
    Feature& feature,
    const string& description)
{
    feature.add_option<string>(
        "description",
        "description used to identify evaluator in logs",
        "\"" + description + "\"");
    utils::add_log_options_to_feature(feature);
}

tuple<string, downward::utils::Verbosity> get_evaluator_arguments_from_options(
    const Context& context,
    const Options& opts)
{
    return tuple_cat(
        make_tuple(opts.get<string>(context, "description")),
        utils::get_log_arguments_from_options(context, opts));
}

} // namespace downward::cli