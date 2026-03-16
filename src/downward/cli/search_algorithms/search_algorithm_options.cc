#include "downward/cli/search_algorithms/search_algorithm_options.h"

#include "downward/cli/operator_cost_options.h"

#include "downward/cli/utils/logging_options.h"
#include "downward/cli/utils/rng_options.h"

#include "language/plugins/internal_function_definition.h"

#include "downward/utils/logging.h"

#include "downward/operator_cost.h"

using namespace std;

namespace downward::cli {

/* TODO: merge this into add_options_to_feature when all search
         algorithms support pruning.

   Method doesn't belong here because it's only useful for certain derived
   classes.
   TODO: Figure out where it belongs and move it there. */
std::size_t add_search_pruning_options_to_feature(
    language::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index)
{
    feature.make_optional_argument_with_default(
        start_index,
        "pruning",
        "null()",
        "Pruning methods can prune or reorder the set of applicable operators "
        "in "
        "each state and thereby influence the number and order of successor "
        "states "
        "that are considered.");
    return 1;
}

std::size_t add_search_algorithm_options_to_feature(
    language::plugins::InternalFunctionDefinitionBase& feature,
    const string& description,
    std::size_t start_index)
{
    auto num = add_cost_type_options_to_feature(feature, start_index);
    start_index += num;
    feature.make_optional_argument_with_default(
        start_index,
        "bound",
        "infinity()",
        "exclusive depth bound on g-values. Cutoffs are always performed "
        "according to "
        "the real cost, regardless of the cost_type parameter");
    feature.make_optional_argument_with_default(
        start_index + 1,
        "max_time",
        "seconds_max()",
        "maximum time in seconds the search is allowed to run for. The "
        "timeout is only checked after each complete search step "
        "(usually a node expansion), so the actual runtime can be arbitrarily "
        "longer. Therefore, this parameter should not be used for "
        "time-limiting "
        "experiments. Timed-out searches are treated as failed searches, "
        "just like incomplete search algorithms that exhaust their search "
        "space.");
    feature.make_optional_argument_with_default(
        start_index + 2,
        "description",
        "\"" + description + "\"",
        "description used to identify search algorithm in logs");
    auto num2 = utils::add_log_options_to_feature(feature, start_index + 3);
    return num + 3 + num2;
}

/* Method doesn't belong here because it's only useful for certain derived
   classes.
   TODO: Figure out where it belongs and move it there. */
std::size_t add_successors_order_options_to_feature(
    language::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index)
{
    feature.document_note(
        "Successor ordering",
        "When using randomize_successors=true and "
        "preferred_successors_first=true, randomization happens before "
        "preferred operators are moved to the front.");

    feature.make_optional_argument_with_default(
        start_index,
        "randomize_successors",
        "false",
        "randomize the order in which successors are generated");
    feature.make_optional_argument_with_default(
        start_index + 1,
        "preferred_successors_first",
        "false",
        "consider preferred operators first");
    auto n = utils::add_rng_options_to_feature(feature, start_index + 2);
    return 2 + n;
}

} // namespace downward::cli