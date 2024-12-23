#include "downward_plugins/merge_and_shrink/merge_strategy_options.h"

#include "downward_plugins/utils/logging_options.h"

#include "downward_plugins/plugins/plugin.h"

using namespace std;

using namespace downward_plugins::plugins;
using namespace downward_plugins::utils;

namespace downward_plugins::merge_and_shrink {

void add_merge_strategy_options_to_feature(Feature& feature)
{
    add_log_options_to_feature(feature);
}

tuple<::utils::Verbosity>
get_merge_strategy_arguments_from_options(const Options& opts)
{
    return get_log_arguments_from_options(opts);
}

} // namespace downward_plugins::merge_and_shrink
