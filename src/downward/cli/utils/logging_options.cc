#include "downward/cli/utils/logging_options.h"

#include "language/plugins/internal_function_definition.h"

#include "downward/utils/logging.h"

using namespace std;

using downward::utils::Verbosity;

namespace downward::cli::utils {

std::size_t add_log_options_to_feature(
    language::plugins::InternalFunctionDefinitionBase& feature,
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
