#include "downward/cli/evaluators/evaluator_options.h"

#include "downward/cli/utils/logging_options.h"

#include "language/ast/internal_function_definition.h"

#include "downward/utils/logging.h"

using namespace std;

namespace downward::cli {

std::size_t add_evaluator_options_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
    const string& description,
    std::size_t start_index)
{
    feature.make_optional_argument_with_default(
        start_index,
        "description",
        "\"" + description + "\"",
        "description used to identify evaluator in logs");
    const auto n = utils::add_log_options_to_feature(feature, start_index + 1);

    return n + 1;
}

} // namespace downward::cli