#include "downward/cli/merge_and_shrink/merge_scoring_function_total_order_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/merge_and_shrink/merge_scoring_function_total_order.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

using namespace std;
using namespace downward::utils;
using namespace downward::merge_and_shrink;

using namespace downward::cli::plugins;

using downward::cli::utils::add_rng_options_to_feature;

namespace {
class MergeScoringFunctionTotalOrderFeature
    : public SharedTypedFeature<
          MergeScoringFunction,
          AtomicTSOrder,
          ProductTSOrder,
          bool,
          int> {
public:
    MergeScoringFunctionTotalOrderFeature()
        : TypedFeature(
              "total_order",
              &MergeScoringFunctionTotalOrderFeature::func)
    {
        document_title("Total order");
        document_synopsis(
            "This scoring function computes a total order on the merge "
            "candidates, "
            "based on the specified options. The score for each merge "
            "candidate "
            "correponds to its position in the order. This scoring function is "
            "mainly intended as tie-breaking, and has been introduced in the "
            "following paper:" +
            format_conference_reference(
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

        make_optional_argument_with_default(
            0,
            "atomic_ts_order",
            "reverse_level",
            "The order in which atomic transition systems are considered when "
            "considering pairs of potential merges.");

        make_optional_argument_with_default(
            1,
            "product_ts_order",
            "new_to_old",
            "The order in which product transition systems are considered when "
            "considering pairs of potential merges.");

        make_optional_argument_with_default(
            2,
            "atomic_before_product",
            "false",
            "Consider atomic transition systems before composite ones iff "
            "true.");

        add_rng_options_to_feature(*this, 3);
    }

    static shared_ptr<MergeScoringFunction> func(
        const Context&,
        AtomicTSOrder atomic_ts_order,
        ProductTSOrder product_ts_order,
        bool atomic_before_product,
        int random_seed)
    {
        return make_shared_from_arg_tuples<MergeScoringFunctionTotalOrder>(
            atomic_ts_order,
            product_ts_order,
            atomic_before_product,
            random_seed);
    }
};
} // namespace

namespace downward::cli::merge_and_shrink {

void add_merge_scoring_function_total_order_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_enum_plugin<AtomicTSOrder>(
        {{"reverse_level", "the variable order of Fast Downward"},
         {"level", "opposite of reverse_level"},
         {"random", "a randomized order"}});

    n.insert_enum_plugin<ProductTSOrder>(
        {{"old_to_new",
          "consider composite transition systems from oldest to most recent"},
         {"new_to_old", "opposite of old_to_new"},
         {"random", "a randomized order"}});

    n.insert_feature_plugin<MergeScoringFunctionTotalOrderFeature>();
}

} // namespace downward::cli::merge_and_shrink
