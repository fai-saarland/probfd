#include "downward/cli/plugins/plugin.h"

#include "downward/merge_and_shrink/merge_scoring_function_dfp.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace merge_and_shrink;
using namespace utils;

using namespace downward::cli::plugins;

namespace {

class MergeScoringFunctionDFPFeature
    : public TypedFeature<MergeScoringFunction, MergeScoringFunctionDFP> {
public:
    MergeScoringFunctionDFPFeature()
        : TypedFeature("dfp")
    {
        document_title("DFP scoring");
        document_synopsis(
            "This scoring function computes the 'DFP' score as descrdibed in "
            "the "
            "paper \"Directed model checking with distance-preserving "
            "abstractions\" "
            "by Draeger, Finkbeiner and Podelski (SPIN 2006), adapted to "
            "planning in "
            "the following paper:" +
            utils::format_conference_reference(
                {"Silvan Sievers", "Martin Wehrle", "Malte Helmert"},
                "Generalized Label Reduction for Merge-and-Shrink Heuristics",
                "https://ai.dmi.unibas.ch/papers/sievers-et-al-aaai2014.pdf",
                "Proceedings of the 28th AAAI Conference on Artificial"
                " Intelligence (AAAI 2014)",
                "2358-2366",
                "AAAI Press",
                "2014"));

        document_note(
            "Note",
            "To obtain the configurations called DFP-B-50K described in the "
            "paper, "
            "use the following configuration of the merge-and-shrink heuristic "
            "and adapt the tie-breaking criteria of {{{total_order}}} as "
            "desired:\n"
            "{{{\nmerge_and_shrink(merge_strategy=merge_stateless(merge_"
            "selector="
            "score_based_filtering(scoring_functions=[goal_relevance,dfp,total_"
            "order("
            "atomic_ts_order=reverse_level,product_ts_order=new_to_old,"
            "atomic_before_product=true)])),shrink_strategy=shrink_"
            "bisimulation("
            "greedy=false),label_reduction=exact(before_shrinking=true,"
            "before_merging=false),max_states=50000,threshold_before_merge=1)"
            "\n}}}");
    }

    virtual shared_ptr<MergeScoringFunctionDFP>
    create_component(const Options&, const Context&) const override
    {
        return make_shared<MergeScoringFunctionDFP>();
    }
};

FeaturePlugin<MergeScoringFunctionDFPFeature> _plugin;

} // namespace
