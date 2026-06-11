#include "downward_cli/landmarks/landmark_factory_options.h"

#include "language/plugins/plugin.h"

#include "downward_cli/utils/logging_options.h"

#include "downward/landmarks/landmark_factory.h"

using namespace std;
using namespace downward::landmarks;
using namespace downward::utils;

using namespace language;
using namespace language::plugins;

using downward::cli::utils::add_log_options_to_feature;
using downward::cli::utils::get_log_arguments_from_options;

namespace downward::cli::landmarks {

void add_landmark_factory_options_to_feature(Feature& feature)
{
    add_log_options_to_feature(feature);
}

tuple<Verbosity> get_landmark_factory_arguments_from_options(
    const Context& context,
    const Options& opts)
{
    return get_log_arguments_from_options(context, opts);
}

void add_use_orders_option_to_feature(Feature& feature)
{
    feature.add_option<bool>(
        "use_orders",
        "use orders between landmarks",
        "true");
}

bool get_use_orders_arguments_from_options(
    const Context& context,
    const Options& opts)
{
    return opts.get<bool>(context, "use_orders");
}

} // namespace downward::cli::landmarks
