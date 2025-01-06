#include "downward/cli/evaluator_options.h"

#include "downward/cli/utils/logging_options.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/utils/logging.h"

using namespace std;

namespace downward::cli {

void add_evaluator_options_to_feature(
    plugins::Feature& feature,
    const string& description)
{
    feature.add_option<string>(
        "description",
        "description used to identify evaluator in logs",
        "\"" + description + "\"");
    utils::add_log_options_to_feature(feature);
}

tuple<string, ::utils::Verbosity>
get_evaluator_arguments_from_options(const plugins::Options& opts)
{
    return tuple_cat(
        make_tuple(opts.get<string>("description")),
        utils::get_log_arguments_from_options(opts));
}

} // namespace downward::cli