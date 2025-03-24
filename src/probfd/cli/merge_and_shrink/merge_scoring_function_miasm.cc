#include "probfd/merge_and_shrink/merge_scoring_function_miasm.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/shrink_strategy.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/cli/merge_and_shrink/merge_and_shrink_algorithm_options.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

#include "downward/cli/plugins/plugin.h"

using namespace std;
using namespace downward::cli::plugins;
using namespace probfd::merge_and_shrink;
using namespace probfd::cli::merge_and_shrink;

namespace {

class MergeScoringFunctionMIASMFeature
    : public TypedFeature<MergeScoringFunction, MergeScoringFunctionMIASM> {
public:
    MergeScoringFunctionMIASMFeature()
        : TypedFeature("psf_miasm")
    {
        document_title("MIASM");
        document_synopsis(
            "This scoring function favors merging transition systems such that "
            "in "
            "their product, there are many dead states, which can then be "
            "pruned "
            "without sacrificing information. In particular, the score it "
            "assigns "
            "to a product is the ratio of alive states to the total number of "
            "states. To compute this score, this class thus computes the "
            "product "
            "of all pairs of transition systems, potentially copying and "
            "shrinking "
            "the transition systems before if otherwise their product would "
            "exceed "
            "the specified size limits. A stateless merge strategy using this "
            "scoring function is called dyn-MIASM (nowadays also called "
            "sbMIASM "
            "for score-based MIASM) and is described in the following paper:" +
            utils::format_conference_reference(
                {"Silvan Sievers", "Martin Wehrle", "Malte Helmert"},
                "An Analysis of Merge Strategies for Merge-and-Shrink "
                "Heuristics",
                "https://ai.dmi.unibas.ch/papers/sievers-et-al-icaps2016.pdf",
                "Proceedings of the 26th International Conference on Planning "
                "and "
                "Scheduling (ICAPS 2016)",
                "2358-2366",
                "AAAI Press",
                "2016"));

        // TODO: use shrink strategy and limit options from
        // MergeAndShrinkHeuristic instead of having the identical options here
        // again.
        add_option<shared_ptr<ShrinkStrategy>>(
            "shrink_strategy",
            "We recommend setting this to match the shrink strategy "
            "configuration "
            "given to {{{merge_and_shrink}}}, see note below.");
        add_transition_system_size_limit_options_to_feature(*this);

        document_note(
            "Note",
            "To obtain the configurations called dyn-MIASM described in the "
            "paper, "
            "use the following configuration of the merge-and-shrink heuristic "
            "and adapt the tie-breaking criteria of {{{total_order}}} as "
            "desired:\n"
            "{{{\nmerge_and_shrink(merge_strategy=merge_stateless(merge_"
            "selector="
            "score_based_filtering(scoring_functions=[sf_miasm(shrink_strategy="
            "shrink_bisimulation(greedy=false),max_states=50000,"
            "threshold_before_merge=1),total_order(atomic_ts_order=reverse_"
            "level,"
            "product_ts_order=new_to_old,atomic_before_product=true)])),"
            "shrink_strategy=shrink_bisimulation(greedy=false),label_reduction="
            "exact(before_shrinking=true,before_merging=false),max_states="
            "50000,"
            "threshold_before_merge=1)\n}}}");
        document_note(
            "Note",
            "Unless you know what you are doing, we recommend using the same "
            "options related to shrinking for {{{sf_miasm}}} as for {{{"
            "merge_and_shrink}}}, i.e. the options {{{shrink_strategy}}}, {{{"
            "max_states}}}, and {{{threshold_before_merge}}} should be set "
            "identically. Furthermore, as this scoring function maximizes the "
            "amount of possible pruning, merge-and-shrink should be configured "
            "to "
            "use full pruning, i.e. {{{prune_unreachable_states=true}}} and {{{"
            "prune_irrelevant_states=true}}} (the default).");

        add_option<bool>(
            "use_caching",
            "Cache scores for merge candidates. IMPORTANT! This only works "
            "under the assumption that the merge-and-shrink algorithm only "
            "uses exact label reduction and does not (non-exactly) shrink "
            "factors other than those being merged in the current iteration. "
            "In this setting, the MIASM score of a merge candidate is constant "
            "over merge-and-shrink iterations. If caching is enabled, only the "
            "scores for the new merge candidates need to be computed.",
            "true");
    }

    shared_ptr<MergeScoringFunctionMIASM> create_component(
        const Options& options,
        const utils::Context& context) const override
    {
        Options options_copy(options);
        handle_shrink_limit_options_defaults(options_copy, context);

        return make_shared<MergeScoringFunctionMIASM>(
            options.get<bool>("use_caching"),
            options.get<shared_ptr<ShrinkStrategy>>("shrink_strategy"),
            options.get<int>("max_states"),
            options.get<int>("max_states_before_merge"),
            options.get<int>("threshold_before_merge"));
    }
};

FeaturePlugin<MergeScoringFunctionMIASMFeature> _plugin;

}
