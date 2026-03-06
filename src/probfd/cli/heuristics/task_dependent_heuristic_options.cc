#include "probfd/cli/heuristics/task_dependent_heuristic_options.h"

#include "downward/cli/utils/logging_options.h"

using namespace language::parser;

using downward::cli::utils::add_log_options_to_feature;

namespace probfd::cli::heuristics {

std::size_t add_task_dependent_heuristic_options_to_feature(
    InternalFunctionDefinitionBase& feature,
    std::size_t start_index)
{
    return add_log_options_to_feature(feature, start_index);
}

} // namespace probfd::cli::heuristics
