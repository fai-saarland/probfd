#include "downward/cli/search_algorithms/eager_search_options.h"

#include "downward/cli/search_algorithms/search_algorithm_options.h"

using namespace std;

using downward::cli::add_search_algorithm_options_to_feature;
using downward::cli::add_search_pruning_options_to_feature;

namespace downward::cli::eager_search {

std::size_t add_eager_search_options_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
    const string& description,
    std::size_t start_index)
{
    const auto n = add_search_pruning_options_to_feature(feature, start_index);
    const auto n2 = add_search_algorithm_options_to_feature(
        feature,
        description,
        start_index + n);
    return n + n2;
}

} // namespace downward::cli::eager_search
