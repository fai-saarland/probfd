#include "downward/cli/merge_and_shrink/merge_strategy_options.h"

#include "downward/cli/utils/logging_options.h"

#include "downward/cli/plugins/plugin.h"

using namespace std;

using namespace downward::cli::plugins;
using namespace downward::cli::utils;

namespace downward::cli::merge_and_shrink {

void add_merge_strategy_options_to_feature(Feature& feature)
{
    add_log_options_to_feature(feature);
}

tuple<::utils::Verbosity>
get_merge_strategy_arguments_from_options(const Options& opts)
{
    return get_log_arguments_from_options(opts);
}

} // namespace downward::cli::merge_and_shrink
