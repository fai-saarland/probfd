#include "probfd/cli/merge_and_shrink/merge_tree_factory_options.h"

#include "probfd/merge_and_shrink/merge_tree.h"

#include "downward/utils/logging.h"

#include "downward/cli/utils/rng_options.h"

#include "language/ast/internal_function_definition.h"

using namespace std;
using namespace language::parser;
using namespace probfd::merge_and_shrink;

namespace probfd::cli::merge_and_shrink {

std::size_t add_merge_tree_factory_options_to_feature(
    InternalFunctionDefinitionBase& feature,
    std::size_t start_index)
{
    const int n =
        downward::cli::utils::add_rng_options_to_feature(feature, start_index);

    feature.make_optional_argument_with_default(
        start_index + n,
        "update_option",
        "use_random",
        "When the merge tree is used within another merge strategy, how "
        "should it be updated when a merge different to a merge from the "
        "tree is performed.");

    return n + 1;
}

} // namespace probfd::cli::merge_and_shrink