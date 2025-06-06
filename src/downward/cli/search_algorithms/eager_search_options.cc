#include "downward/cli/search_algorithms/eager_search_options.h"

#include "downward/cli/plugins/options.h"

#include "downward/cli/open_list_options.h"
#include "downward/cli/search_algorithm_options.h"

#include "downward/utils/logging.h"

#include <memory>

using namespace std;

using downward::cli::add_search_algorithm_options_to_feature;
using downward::cli::add_search_pruning_options_to_feature;

using downward::cli::get_search_algorithm_arguments_from_options;
using downward::cli::get_search_pruning_arguments_from_options;

namespace downward::cli::eager_search {

void add_eager_search_options_to_feature(
    plugins::Feature& feature,
    const string& description)
{
    add_search_pruning_options_to_feature(feature);
    // We do not add a lazy_evaluator options here
    // because it is only used for astar but not the other plugins.
    add_search_algorithm_options_to_feature(feature, description);
}

tuple<
    shared_ptr<PruningMethod>,
    shared_ptr<Evaluator>,
    OperatorCost,
    int,
    double,
    string,
    utils::Verbosity>
get_eager_search_arguments_from_options(const plugins::Options& opts)
{
    return tuple_cat(
        get_search_pruning_arguments_from_options(opts),
        make_tuple(opts.get<shared_ptr<Evaluator>>("lazy_evaluator", nullptr)),
        get_search_algorithm_arguments_from_options(opts));
}

} // namespace downward::cli::eager_search
