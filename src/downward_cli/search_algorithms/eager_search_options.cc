#include "downward_cli/search_algorithms/eager_search_options.h"

#include "language/plugins/options.h"

#include "downward_cli/open_lists/open_list_options.h"
#include "downward_cli/search_algorithms/search_algorithm_options.h"

#include "downward/utils/logging.h"

#include <memory>

using namespace std;

using namespace language;
using namespace language::plugins;

using downward::cli::add_search_algorithm_options_to_feature;
using downward::cli::add_search_pruning_options_to_feature;

using downward::cli::get_search_algorithm_arguments_from_options;
using downward::cli::get_search_pruning_arguments_from_options;

namespace downward::cli::eager_search {

void add_eager_search_options_to_feature(
    Feature& feature,
    const string& description)
{
    add_search_pruning_options_to_feature(feature);
    add_search_algorithm_options_to_feature(feature, description);
}

tuple<
    shared_ptr<PruningMethod>,
    OperatorCost,
    int,
    downward::utils::Duration,
    string,
    utils::Verbosity>
get_eager_search_arguments_from_options(
    const Context& context,
    const Options& opts)
{
    return tuple_cat(
        get_search_pruning_arguments_from_options(context, opts),
        get_search_algorithm_arguments_from_options(context, opts));
}

} // namespace downward::cli::eager_search
