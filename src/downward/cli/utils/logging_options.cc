#include "downward/cli/utils/logging_options.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/utils/logging.h"

using namespace std;

using downward::utils::Verbosity;

namespace downward::cli::utils {

void add_log_options_to_feature(plugins::Feature& feature)
{
    feature.add_option<Verbosity>(
        "verbosity",
        "Option to specify the verbosity level.",
        "normal");
}

tuple<Verbosity> get_log_arguments_from_options(const plugins::Options& opts)
{
    return make_tuple<Verbosity>(opts.get<Verbosity>("verbosity"));
}

plugins::TypedEnumPlugin<Verbosity> _enum_plugin(
    {{"silent", "only the most basic output"},
     {"normal", "relevant information to monitor progress"},
     {"verbose", "full output"},
     {"debug", "like verbose with additional debug output"}});

} // namespace downward::cli::utils
