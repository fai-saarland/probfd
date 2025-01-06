#include "downward/cli/pdbs/pattern_generator_options.h"

#include "downward/cli/utils/logging_options.h"

#include "downward/cli/plugins/plugin.h"

using namespace std;

using namespace downward::cli::plugins;

namespace downward::cli::pdbs {

void add_generator_options_to_feature(Feature& feature)
{
    utils::add_log_options_to_feature(feature);
}

tuple<::utils::Verbosity>
get_generator_arguments_from_options(const Options& opts)
{
    return utils::get_log_arguments_from_options(opts);
}

} // namespace downward::cli::pdbs
