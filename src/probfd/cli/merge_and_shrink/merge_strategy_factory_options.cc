#include "probfd/cli/merge_and_shrink/merge_strategy_factory_options.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/cli/utils/logging_options.h"

using namespace std;
using namespace downward::cli::plugins;
using namespace downward;

namespace probfd::cli::merge_and_shrink {

void add_merge_strategy_options_to_feature(Feature& feature)
{
    downward::cli::utils::add_log_options_to_feature(feature);
}

std::tuple<utils::Verbosity>
get_merge_strategy_args_from_options(const Options& options)
{
    return downward::cli::utils::get_log_arguments_from_options(options);
}

} // namespace
