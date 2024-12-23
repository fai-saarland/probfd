#include "downward_plugins/search_algorithm_options.h"

#include "downward_plugins/operator_cost_options.h"

#include "downward_plugins/utils/logging_options.h"
#include "downward_plugins/utils/rng_options.h"

#include "downward_plugins/plugins/plugin.h"

#include "downward/utils/logging.h"

#include "downward/operator_cost.h"

using namespace std;

namespace downward_plugins {

/* TODO: merge this into add_options_to_feature when all search
         algorithms support pruning.

   Method doesn't belong here because it's only useful for certain derived
   classes.
   TODO: Figure out where it belongs and move it there. */
void add_search_pruning_options_to_feature(plugins::Feature& feature)
{
    feature.add_option<shared_ptr<PruningMethod>>(
        "pruning",
        "Pruning methods can prune or reorder the set of applicable operators "
        "in "
        "each state and thereby influence the number and order of successor "
        "states "
        "that are considered.",
        "null()");
}

tuple<shared_ptr<PruningMethod>>
get_search_pruning_arguments_from_options(const plugins::Options& opts)
{
    return make_tuple(opts.get<shared_ptr<PruningMethod>>("pruning"));
}

void add_search_algorithm_options_to_feature(
    plugins::Feature& feature,
    const string& description)
{
    add_cost_type_options_to_feature(feature);
    feature.add_option<int>(
        "bound",
        "exclusive depth bound on g-values. Cutoffs are always performed "
        "according to "
        "the real cost, regardless of the cost_type parameter",
        "infinity");
    feature.add_option<double>(
        "max_time",
        "maximum time in seconds the search is allowed to run for. The "
        "timeout is only checked after each complete search step "
        "(usually a node expansion), so the actual runtime can be arbitrarily "
        "longer. Therefore, this parameter should not be used for "
        "time-limiting "
        "experiments. Timed-out searches are treated as failed searches, "
        "just like incomplete search algorithms that exhaust their search "
        "space.",
        "infinity");
    feature.add_option<string>(
        "description",
        "description used to identify search algorithm in logs",
        "\"" + description + "\"");
    utils::add_log_options_to_feature(feature);
}

tuple<OperatorCost, int, double, string, ::utils::Verbosity>
get_search_algorithm_arguments_from_options(const plugins::Options& opts)
{
    return tuple_cat(
        get_cost_type_arguments_from_options(opts),
        make_tuple(
            opts.get<int>("bound"),
            opts.get<double>("max_time"),
            opts.get<string>("description")),
        utils::get_log_arguments_from_options(opts));
}

/* Method doesn't belong here because it's only useful for certain derived
   classes.
   TODO: Figure out where it belongs and move it there. */
void add_successors_order_options_to_feature(plugins::Feature& feature)
{
    vector<string> options;
    feature.add_option<bool>(
        "randomize_successors",
        "randomize the order in which successors are generated",
        "false");
    feature.add_option<bool>(
        "preferred_successors_first",
        "consider preferred operators first",
        "false");
    feature.document_note(
        "Successor ordering",
        "When using randomize_successors=true and "
        "preferred_successors_first=true, randomization happens before "
        "preferred operators are moved to the front.");
    utils::add_rng_options_to_feature(feature);
}

tuple<bool, bool, int>
get_successors_order_arguments_from_options(const plugins::Options& opts)
{
    return tuple_cat(
        make_tuple(
            opts.get<bool>("randomize_successors"),
            opts.get<bool>("preferred_successors_first")),
        utils::get_rng_arguments_from_options(opts));
}

} // namespace downward_plugins