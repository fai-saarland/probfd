#include "downward/cli/merge_and_shrink/label_reduction_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/merge_and_shrink/factored_transition_system.h"
#include "downward/merge_and_shrink/label_reduction.h"
#include "downward/merge_and_shrink/transition_system.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace downward::utils;
using namespace downward::merge_and_shrink;

using namespace downward::cli::plugins;

using downward::cli::utils::add_rng_options_to_feature;

namespace {
class LabelReductionFeature
    : public SharedTypedFeature<
          LabelReduction,
          bool,
          bool,
          LabelReductionMethod,
          LabelReductionSystemOrder,
          int> {
public:
    LabelReductionFeature()
        : TypedFeature("exact", &LabelReductionFeature::func)
    {
        document_title("Exact generalized label reduction");
        document_synopsis(
            "This class implements the exact generalized label reduction "
            "described in the following paper:" +
            format_conference_reference(
                {"Silvan Sievers", "Martin Wehrle", "Malte Helmert"},
                "Generalized Label Reduction for Merge-and-Shrink Heuristics",
                "https://ai.dmi.unibas.ch/papers/sievers-et-al-aaai2014.pdf",
                "Proceedings of the 28th AAAI Conference on Artificial"
                " Intelligence (AAAI 2014)",
                "2358-2366",
                "AAAI Press",
                "2014"));

        make_required_argument(
            0,
            "before_shrinking",
            "apply label reduction before shrinking");

        make_required_argument(
            1,
            "before_merging",
            "apply label reduction before merging");

        make_optional_argument_with_default(
            2,
            "method",
            "all_transition_systems_with_fixpoint",
            "Label reduction method. See the AAAI14 paper by "
            "Sievers et al. for explanation of the default label "
            "reduction method and the 'combinable relation' ."
            "Also note that you must set at least one of the "
            "options reduce_labels_before_shrinking or "
            "reduce_labels_before_merging in order to use "
            "the chosen label reduction configuration.");

        make_optional_argument_with_default(
            3,
            "system_order",
            "random",
            "Order of transition systems for the label reduction "
            "methods that iterate over the set of all transition "
            "systems. Only useful for the choices "
            "all_transition_systems and "
            "all_transition_systems_with_fixpoint for the option "
            "label_reduction_method.");

        // Add random_seed option.
        add_rng_options_to_feature(*this, 4);
    }

    static shared_ptr<LabelReduction> func(
        bool before_shrinking,
        bool before_merging,
        LabelReductionMethod method,
        LabelReductionSystemOrder system_order,
        int random_seed)
    {
        return make_shared_from_arg_tuples<LabelReduction>(
            before_shrinking,
            before_merging,
            method,
            system_order,
            random_seed);
    }
};
} // namespace

namespace downward::cli::merge_and_shrink {

void add_label_reduction_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_category_plugin<LabelReduction>(
        "LabelReduction",
        "This page describes the current single 'option' for "
        "label reduction.");

    n.insert_enum_plugin<LabelReductionMethod>(
        {{"two_transition_systems",
          "compute the 'combinable relation' only for the two transition "
          "systems being merged next"},
         {"all_transition_systems",
          "compute the 'combinable relation' for labels once for every "
          "transition system and reduce labels"},
         {"all_transition_systems_with_fixpoint",
          "keep computing the 'combinable relation' for labels iteratively "
          "for all transition systems until no more labels can be reduced"}});

    n.insert_enum_plugin<LabelReductionSystemOrder>(
        {{"regular",
          "transition systems are considered in the order given in the planner "
          "input if atomic and in the order of their creation if composite."},
         {"reverse", "inverse of regular"},
         {"random", "random order"}});
}

void add_label_reduction_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<LabelReductionFeature>();
}

} // namespace downward::cli::merge_and_shrink
