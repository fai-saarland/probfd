#include "downward_cli/merge_and_shrink/merge_strategy_options.h"

#include "downward_cli/utils/logging_options.h"

#include "language/plugins/plugin.h"

using namespace std;

using namespace language;
using namespace language::plugins;

using namespace downward::cli::utils;

namespace downward::cli::merge_and_shrink {

void add_merge_strategy_options_to_feature(Feature& feature)
{
    add_log_options_to_feature(feature);
}

tuple<downward::utils::Verbosity> get_merge_strategy_arguments_from_options(
    const Context& context,
    const Options& opts)
{
    return get_log_arguments_from_options(context, opts);
}

} // namespace downward::cli::merge_and_shrink
