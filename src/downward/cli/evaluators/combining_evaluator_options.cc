#include "downward/cli/evaluators/combining_evaluator_options.h"

#include "language/plugins/internal_function_definition.h"

#include "downward/cli/evaluators/evaluator_options.h"

using namespace std;

using namespace language::plugins;

namespace downward::cli::combining_evaluator {

std::size_t add_combining_evaluator_options_to_feature(
    InternalFunctionDefinitionBase& feature,
    const string& description,
    std::size_t start_index)
{
    feature.make_required_argument(
        start_index,
        "evals",
        "at least one evaluator");
    const auto n =
        add_evaluator_options_to_feature(feature, description, start_index + 1);

    return n + 1;
}

} // namespace downward::cli::combining_evaluator
