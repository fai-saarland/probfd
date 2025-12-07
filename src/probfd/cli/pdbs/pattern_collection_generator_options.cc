#include "probfd/cli/pdbs/pattern_collection_generator_options.h"

#include "language/plugins/plugin.h"

#include "downward/cli/utils/logging_options.h"

using namespace downward::cli::plugins;

using downward::cli::utils::add_log_options_to_feature;

namespace probfd::cli::pdbs {

std::size_t add_pattern_collection_generator_options_to_feature(
    InternalFunctionDefinitionBase& feature,
    std::size_t start_index)
{
    return add_log_options_to_feature(feature, start_index);
}

} // namespace probfd::cli::pdbs
