#include "downward/cli/heuristics/heuristic_options.h"

#include "downward/cli/evaluators/evaluator_options.h"

#include "language/plugins/plugin.h"

#include "downward/utils/logging.h"

#include "downward/tasks/root_task.h"

using namespace std;

namespace downward::cli {

std::size_t add_heuristic_options_to_feature(
    plugins::InternalFunctionDefinitionBase& feature,
    const string& description,
    std::size_t start_index)
{
    feature.make_optional_argument_with_default(
        start_index,
        "transform",
        "no_transform()",
        "Optional task transformation for the heuristic.");
    feature.make_optional_argument_with_default(
        start_index + 1,
        "cache_estimates",
        "true",
        "cache heuristic estimates");
    const auto n = add_evaluator_options_to_feature(feature, description, start_index + 2);

    return n + 2;
}

} // namespace downward::cli