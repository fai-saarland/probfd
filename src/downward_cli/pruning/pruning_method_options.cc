#include "downward_cli/pruning/pruning_method_options.h"

#include "downward_cli/utils/logging_options.h"

#include "language/plugins/plugin.h"

#include "downward/utils/logging.h"

using namespace std;

using namespace language;

namespace downward::cli {

void add_pruning_options_to_feature(plugins::Feature& feature)
{
    utils::add_log_options_to_feature(feature);
    feature.document_note(
        "Note on verbosity parameter",
        "Setting verbosity to verbose or higher enables time measurements "
        "in each call to prune_operators for a given state. This induces a "
        "significant overhead, up to 30% in configurations like blind search "
        "with the no pruning method (`null`). We recommend using at most "
        "normal verbosity for running experiments.");
}

tuple<downward::utils::Verbosity> get_pruning_arguments_from_options(
    const Context& context,
    const plugins::Options& opts)
{
    return utils::get_log_arguments_from_options(context, opts);
}

} // namespace downward::cli
