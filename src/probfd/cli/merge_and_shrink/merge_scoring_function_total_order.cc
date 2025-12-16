#include "probfd/cli/merge_and_shrink/merge_scoring_function_total_order.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/utils/rng_options.h"

#include "probfd/merge_and_shrink/merge_scoring_function_total_order.h"

#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

using namespace std;
using namespace language::plugins;
using namespace downward;
using namespace probfd::merge_and_shrink;

namespace {

InternalFunctionDefinitionBase& add_merge_scoring_function_total_order_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "ptotal_order",
        &language::plugins::construct_shared<
            MergeScoringFunction,
            MergeScoringFunctionTotalOrder,
            AtomicTSOrder,
            ProductTSOrder,
            bool,
            std::shared_ptr<utils::RandomNumberGenerator>>);

    f.document_title("Total order");
    f.document_synopsis(
        "This scoring function computes a total order on the merge "
        "candidates, "
        "based on the specified options. The score for each merge "
        "candidate "
        "correponds to its position in the order. This scoring function is "
        "mainly intended as tie-breaking, and has been introduced in the "
        "following paper:" +
        utils::format_conference_reference(
            {"Silvan Sievers", "Martin Wehrle", "Malte Helmert"},
            "An Analysis of Merge Strategies for Merge-and-Shrink "
            "Heuristics",
            "https://ai.dmi.unibas.ch/papers/sievers-et-al-icaps2016.pdf",
            "Proceedings of the 26th International Conference on Automated "
            "Planning and Scheduling (ICAPS 2016)",
            "294-298",
            "AAAI Press",
            "2016") +
        "Furthermore, using the atomic_ts_order option, this scoring "
        "function, "
        "if used alone in a score based filtering merge selector, can be "
        "used "
        "to emulate the corresponding (precomputed) linear merge "
        "strategies "
        "reverse level/level (independently of the other options).");

    f.make_optional_argument_with_default(
        0,
        "atomic_ts_order",
        "reverse_level",
        "The order in which atomic transition systems are considered when "
        "considering pairs of potential merges.");

    f.make_optional_argument_with_default(
        1,
        "product_ts_order",
        "new_to_old",
        "The order in which product transition systems are considered when "
        "considering pairs of potential merges.");

    f.make_optional_argument_with_default(
        2,
        "atomic_before_product",
        "false",
        "Consider atomic transition systems before composite ones iff "
        "true.");

    downward::cli::utils::add_rng_options_to_feature(f, 3);

    return f;
}

} // namespace

namespace probfd::cli::merge_and_shrink {

void add_merge_scoring_function_total_order_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_enum_declaration<AtomicTSOrder>(
        {{"reverse_level", "the variable order of Fast Downward"},
         {"level", "opposite of reverse_level"},
         {"random", "a randomized order"}});

    n.insert_enum_declaration<ProductTSOrder>(
        {{"old_to_new",
          "consider composite transition systems from oldest to most recent"},
         {"new_to_old", "opposite of old_to_new"},
         {"random", "a randomized order"}});

    add_merge_scoring_function_total_order_to_namespace(n);
}

} // namespace probfd::cli::merge_and_shrink
