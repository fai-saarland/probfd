#include "probfd/cli/merge_and_shrink/merge_strategy_factory_options.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/cli/utils/logging_options.h"

using namespace std;
using namespace downward::cli::plugins;
using namespace downward;

namespace probfd::cli::merge_and_shrink {

std::size_t
add_merge_strategy_options_to_feature(InternalFunctionDefinitionBase& feature, std::size_t start_index)
{
    return downward::cli::utils::add_log_options_to_feature(
        feature,
        start_index);
}

} // namespace probfd::cli::merge_and_shrink
