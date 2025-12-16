#include "downward/cli/merge_and_shrink/merge_strategy_options.h"

#include "downward/cli/utils/logging_options.h"

#include "language/plugins/internal_function_definition.h"

using namespace std;

using namespace language::plugins;
using namespace downward::cli::utils;

namespace downward::cli::merge_and_shrink {

std::size_t add_merge_strategy_options_to_feature(InternalFunctionDefinitionBase& feature, std::size_t start_index)
{
    return add_log_options_to_feature(feature, start_index);
}

} // namespace downward::cli::merge_and_shrink
