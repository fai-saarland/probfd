#include "probfd/cli/merge_and_shrink/label_reduction_feature.h"

#include "probfd/merge_and_shrink/label_reduction.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"
#include "downward/utils/rng.h"

#include "downward/cli/plugins/options.h"
#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/utils/rng_options.h"

using namespace std;
using namespace downward;
using namespace downward::cli::plugins;

using utils::ExitCode;

using namespace probfd::merge_and_shrink;

namespace {

Feature& add_exact_label_reduction_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "pexact",
        &downward::cli::plugins::make_shared<
            LabelReduction,
            LabelReduction,
            bool,
            bool,
            LabelReductionMethod,
            LabelReductionSystemOrder,
            std::shared_ptr<utils::RandomNumberGenerator>>);

    f.document_title("Exact generalized label reduction");
    f.document_synopsis(
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

    f.make_required_argument(
        0,
        "before_shrinking",
        "apply label reduction before shrinking");

    f.make_required_argument(
        1,
        "before_merging",
        "apply label reduction before merging");

    f.make_optional_argument_with_default(
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

    f.make_optional_argument_with_default(
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
    downward::cli::utils::add_rng_options_to_feature(f, 4);

    return f;
}

} // namespace

namespace probfd::cli::merge_and_shrink {

void add_label_reduction_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_category_plugin<LabelReduction>(
        "PLabelReduction",
        "This page describes the current single 'option' for "
        "label reduction.");
}

void add_label_reduction_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
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

    add_exact_label_reduction_to_namespace(n);
}

} // namespace probfd::cli::merge_and_shrink