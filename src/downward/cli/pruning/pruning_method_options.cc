#include "downward/cli/pruning/pruning_method_options.h"

#include "downward/cli/utils/logging_options.h"

#include "language/plugins/plugin.h"

using namespace std;

namespace downward::cli {

std::size_t add_pruning_options_to_feature(
    plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index)
{
    feature.document_note(
        "Note on verbosity parameter",
        "Setting verbosity to verbose or higher enables time measurements "
        "in each call to prune_operators for a given state. This induces a "
        "significant overhead, up to 30% in configurations like blind search "
        "with the no pruning method (`null`). We recommend using at most "
        "normal verbosity for running experiments.");
    return utils::add_log_options_to_feature(feature, start_index);
}

} // namespace downward::cli
