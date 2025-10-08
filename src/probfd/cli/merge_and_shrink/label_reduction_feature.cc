#include "probfd/cli/merge_and_shrink/label_reduction_feature.h"

#include "probfd/merge_and_shrink/label_reduction.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"
#include "downward/utils/rng.h"

#include "downward/cli/plugins/options.h"
#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/utils/rng_options.h"

using namespace std;
using namespace downward;
using namespace downward::cli::plugins;

using utils::ExitCode;

using namespace probfd::merge_and_shrink;

namespace {
class LabelReductionFeature
    : public SharedTypedFeature<LabelReduction> {
public:
    LabelReductionFeature()
        : SharedTypedFeature("pexact")
    {
        document_title("Exact generalized label reduction");
        document_synopsis(
            "This class implements the exact generalized label reduction "
            "described in the following paper:" +
            utils::format_conference_reference(
                {"Silvan Sievers", "Martin Wehrle", "Malte Helmert"},
                "Generalized Label Reduction for Merge-and-Shrink Heuristics",
                "https://ai.dmi.unibas.ch/papers/sievers-et-al-aaai2014.pdf",
                "Proceedings of the 28th AAAI Conference on Artificial"
                " Intelligence (AAAI 2014)",
                "2358-2366",
                "AAAI Press",
                "2014"));

        add_option<bool>(
            "before_shrinking",
            "apply label reduction before shrinking");
        add_option<bool>(
            "before_merging",
            "apply label reduction before merging");

        add_option<LabelReductionMethod>(
            "method",
            "Label reduction method. See the AAAI14 paper by "
            "Sievers et al. for explanation of the default label "
            "reduction method and the 'combinable relation' ."
            "Also note that you must set at least one of the "
            "options reduce_labels_before_shrinking or "
            "reduce_labels_before_merging in order to use "
            "the chosen label reduction configuration.",
            "all_transition_systems_with_fixpoint");

        add_option<LabelReductionSystemOrder>(
            "system_order",
            "Order of transition systems for the label reduction "
            "methods that iterate over the set of all transition "
            "systems. Only useful for the choices "
            "all_transition_systems and "
            "all_transition_systems_with_fixpoint for the option "
            "label_reduction_method.",
            "random");
        // Add random_seed option.
        downward::cli::utils::add_rng_options_to_feature(*this);
    }

    shared_ptr<LabelReduction>
    create_component(const Options& options, const utils::Context& context)
        const override
    {
        const bool lr_before_shrinking = options.get<bool>("before_shrinking");
        const bool lr_before_merging = options.get<bool>("before_merging");

        if (!lr_before_shrinking && !lr_before_merging) {
            context.error(
                "Please turn on at least one of the options "
                "before_shrinking or before_merging!");
        }

        return make_shared_from_arg_tuples<LabelReduction>(
            lr_before_shrinking,
            lr_before_merging,
            options.get<LabelReductionMethod>("method"),
            options.get<LabelReductionSystemOrder>("system_order"),
            downward::cli::utils::get_rng_arguments_from_options(options));
    }
};

} // namespace

namespace probfd::cli::merge_and_shrink {

void add_label_reduction_features(RawRegistry& raw_registry)
{
    auto& category =
        raw_registry.insert_category_plugin<LabelReduction>("PLabelReduction");
    category.document_synopsis(
        "This page describes the current single 'option' for "
        "label reduction.");

    raw_registry.insert_feature_plugin<LabelReductionFeature>();

    raw_registry.insert_enum_plugin<LabelReductionMethod>(
        {{"two_transition_systems",
          "compute the 'combinable relation' only for the two transition "
          "systems being merged next"},
         {"all_transition_systems",
          "compute the 'combinable relation' for labels once for every "
          "transition system and reduce labels"},
         {"all_transition_systems_with_fixpoint",
          "keep computing the 'combinable relation' for labels iteratively "
          "for all transition systems until no more labels can be reduced"}});

    raw_registry.insert_enum_plugin<LabelReductionSystemOrder>(
        {{"regular",
          "transition systems are considered in the order given in the planner "
          "input if atomic and in the order of their creation if composite."},
         {"reverse", "inverse of regular"},
         {"random", "random order"}});
}

} // namespace probfd::cli::merge_and_shrink