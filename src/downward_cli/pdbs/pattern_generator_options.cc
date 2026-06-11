#include "downward_cli/pdbs/pattern_generator_options.h"

#include "downward_cli/utils/logging_options.h"

#include "language/plugins/plugin.h"

using namespace std;

using namespace language;
using namespace language::plugins;

namespace downward::cli::pdbs {

void add_generator_options_to_feature(Feature& feature)
{
    utils::add_log_options_to_feature(feature);
}

tuple<downward::utils::Verbosity> get_generator_arguments_from_options(
    const Context& context,
    const Options& opts)
{
    return utils::get_log_arguments_from_options(context, opts);
}

} // namespace downward::cli::pdbs
