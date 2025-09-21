#include "probfd/cli/merge_and_shrink/merge_scoring_function_total_order.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/utils/rng_options.h"

#include "probfd/merge_and_shrink/merge_scoring_function_total_order.h"

#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"
#include "downward/utils/rng_options.h"

using namespace std;
using namespace downward::cli::plugins;
using namespace downward;
using namespace probfd::merge_and_shrink;

namespace {
class MergeScoringFunctionTotalOrderFeature
    : public TypedFeature<MergeScoringFunction> {
public:
    MergeScoringFunctionTotalOrderFeature()
        : TypedFeature("ptotal_order")
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

        add_option<AtomicTSOrder>(
            "atomic_ts_order",
            "The order in which atomic transition systems are considered when "
            "considering pairs of potential merges.",
            "reverse_level");

        add_option<ProductTSOrder>(
            "product_ts_order",
            "The order in which product transition systems are considered when "
            "considering pairs of potential merges.",
            "new_to_old");

        add_option<bool>(
            "atomic_before_product",
            "Consider atomic transition systems before composite ones iff "
            "true.",
            "false");

        downward::cli::utils::add_rng_options_to_feature(*this);
    }

protected:
    shared_ptr<MergeScoringFunction>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<MergeScoringFunctionTotalOrder>(
            options.get<AtomicTSOrder>("atomic_ts_order"),
            options.get<ProductTSOrder>("product_ts_order"),
            options.get<bool>("atomic_before_product"),
            downward::cli::utils::get_rng_arguments_from_options(options));
    }
};
} // namespace

namespace probfd::cli::merge_and_shrink {

void add_merge_scoring_function_total_order_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<MergeScoringFunctionTotalOrderFeature>();

    raw_registry.insert_enum_plugin<AtomicTSOrder>(
        {{"reverse_level", "the variable order of Fast Downward"},
         {"level", "opposite of reverse_level"},
         {"random", "a randomized order"}});

    raw_registry.insert_enum_plugin<ProductTSOrder>(
        {{"old_to_new",
          "consider composite transition systems from oldest to most recent"},
         {"new_to_old", "opposite of old_to_new"},
         {"random", "a randomized order"}});
}

} // namespace probfd::cli::merge_and_shrink
