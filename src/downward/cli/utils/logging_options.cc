#include "downward/cli/utils/logging_options.h"

#include "language/ast/internal_function_definition.h"

using namespace std;

namespace downward::cli::utils {

std::size_t add_log_options_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index)
{
    feature.make_optional_argument_with_default(
        start_index,
        "verbosity",
        "normal",
        "Option to specify the verbosity level.");
    return 1;
}

} // namespace downward::cli::utils
