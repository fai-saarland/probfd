#include "downward/cli/merge_and_shrink/shrink_bucket_based_options.h"

#include "language/ast/internal_function_definition.h"

#include "downward/cli/utils/rng_options.h"

using namespace std;

namespace downward::cli::merge_and_shrink {

std::size_t add_shrink_bucket_options_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index)
{
    return utils::add_rng_options_to_feature(feature, start_index);
}

} // namespace downward::cli::merge_and_shrink
