#include "downward/cli/pdbs/pattern_generator_options.h"

#include "downward/cli/utils/logging_options.h"

#include "language/ast/internal_function_definition.h"

using namespace std;

using namespace language::parser;

namespace downward::cli::pdbs {

std::size_t add_generator_options_to_feature(
    InternalFunctionDefinitionBase& feature,
    std::size_t start_index)
{
    return utils::add_log_options_to_feature(feature, start_index);
}

} // namespace downward::cli::pdbs
