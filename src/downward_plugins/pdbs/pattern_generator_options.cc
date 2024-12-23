#include "downward_plugins/pdbs/pattern_generator_options.h"

#include "downward_plugins/utils/logging_options.h"

#include "downward_plugins/plugins/plugin.h"

using namespace std;

using namespace downward_plugins::plugins;

namespace downward_plugins::pdbs {

void add_generator_options_to_feature(Feature& feature)
{
    utils::add_log_options_to_feature(feature);
}

tuple<::utils::Verbosity>
get_generator_arguments_from_options(const Options& opts)
{
    return utils::get_log_arguments_from_options(opts);
}

} // namespace downward_plugins::pdbs
