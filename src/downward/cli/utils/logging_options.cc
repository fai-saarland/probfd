#include "downward/cli/utils/logging_options.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/utils/logging.h"

using namespace std;

using downward::utils::Verbosity;

namespace downward::cli::utils {

void add_log_options_to_feature(plugins::Feature& feature)
{
    feature.add_optional_argument_with_default<Verbosity>(
        "verbosity",
        "normal",
        "Option to specify the verbosity level.");
}

tuple<Verbosity> get_log_arguments_from_options(const plugins::Options& opts)
{
    return make_tuple<Verbosity>(opts.get<Verbosity>("verbosity"));
}

} // namespace downward::cli::utils
