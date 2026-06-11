#include "probfd_cli/pdbs/pattern_generator_options.h"

#include "language/plugins/plugin.h"

#include "downward_cli/utils/logging_options.h"

using namespace language;
using namespace language::plugins;

using downward::cli::utils::add_log_options_to_feature;
using downward::cli::utils::get_log_arguments_from_options;

namespace probfd::cli::pdbs {

void add_pattern_generator_options_to_feature(Feature& feature)
{
    add_log_options_to_feature(feature);
}

PatternGeneratorArgs get_generator_arguments_from_options(
    const Context& context,
    const Options& opts)
{
    return get_log_arguments_from_options(context, opts);
}

} // namespace probfd::cli::pdbs
