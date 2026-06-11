#include "downward_cli/utils/logging_options.h"

#include "language/plugins/plugin.h"

#include "downward/utils/logging.h"

using namespace std;

using namespace language;
using namespace language::plugins;

using downward::utils::Verbosity;

namespace downward::cli::utils {

void add_log_options_to_feature(Feature& feature)
{
    feature.add_option<Verbosity>(
        "verbosity",
        "Option to specify the verbosity level.",
        "normal");
}

tuple<Verbosity>
get_log_arguments_from_options(const Context& context, const Options& opts)
{
    return make_tuple<Verbosity>(opts.get<Verbosity>(context, "verbosity"));
}

} // namespace downward::cli::utils
