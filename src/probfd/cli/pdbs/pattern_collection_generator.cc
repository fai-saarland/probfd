#include "probfd/cli/pdbs/pattern_collection_generator.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/cli/utils/logging_options.h"

using namespace downward::cli::plugins;

using downward::cli::utils::add_log_options_to_feature;
using downward::cli::utils::get_log_arguments_from_options;

namespace probfd::cli::pdbs {

void add_pattern_collection_generator_options_to_feature(Feature& feature)
{
    add_log_options_to_feature(feature);
}

std::tuple<utils::Verbosity>
get_collection_generator_arguments_from_options(const Options& opts)
{
    return get_log_arguments_from_options(opts);
}

} // namespace probfd::cli::pdbs
