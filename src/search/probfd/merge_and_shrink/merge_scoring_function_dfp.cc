#include "probfd/merge_and_shrink/merge_scoring_function_dfp.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/labels.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "downward/plugins/plugin.h"
#include "downward/utils/markup.h"

#include <cassert>

using namespace std;

namespace probfd::merge_and_shrink {

static bool is_self_loop(const Transition& transition)
{
    for (int target : transition.targets) {
        if (target != transition.src) {
            return false;
        }
    }
    return true;
}

static vector<double>
compute_label_ranks(const FactoredTransitionSystem& fts, int index)
{
    const TransitionSystem& ts = fts.get_transition_system(index);
    const Distances& distances = fts.get_distances(index);
    assert(distances.are_goal_distances_computed());
    int num_labels = fts.get_labels().get_num_total_labels();
    // Irrelevant (and inactive, i.e. reduced) labels have a dummy rank of -1
    vector<double> label_ranks(num_labels, -1.0);

    for (const LocalLabelInfo& local_label_info : ts.label_infos()) {
        const LabelGroup& label_group = local_label_info.get_label_group();
        const auto& transitions = local_label_info.get_transitions();

        bool group_relevant =
            (static_cast<int>(transitions.size()) != ts.get_size()) ||
            !std::all_of(transitions.begin(), transitions.end(), is_self_loop);

        double label_rank;

        if (!group_relevant) {
            label_rank = -1.0;
        } else {
            label_rank = std::numeric_limits<double>::infinity();
            for (const Transition& transition : transitions) {
                label_rank =
                    min(label_rank,
                        distances.get_goal_distance(transition.src));
            }
        }

        for (int label : label_group) {
            label_ranks[label] = label_rank;
        }
    }

    return label_ranks;
}

vector<double> MergeScoringFunctionDFP::compute_scores(
    const FactoredTransitionSystem& fts,
    const vector<pair<int, int>>& merge_candidates)
{
    int num_ts = fts.get_size();

    vector<vector<double>> transition_system_label_ranks(num_ts);
    vector<double> scores;
    scores.reserve(merge_candidates.size());

    // Go over all pairs of transition systems and compute their weight.
    for (auto [ts_index1, ts_index2] : merge_candidates) {
        vector<double>& label_ranks1 = transition_system_label_ranks[ts_index1];
        if (label_ranks1.empty()) {
            label_ranks1 = compute_label_ranks(fts, ts_index1);
        }
        vector<double>& label_ranks2 = transition_system_label_ranks[ts_index2];
        if (label_ranks2.empty()) {
            label_ranks2 = compute_label_ranks(fts, ts_index2);
        }
        assert(label_ranks1.size() == label_ranks2.size());

        // Compute the weight associated with this pair
        double pair_weight = std::numeric_limits<double>::infinity();
        for (size_t i = 0; i < label_ranks1.size(); ++i) {
            if (label_ranks1[i] == -1.0 || label_ranks2[i] == -1.0) continue;
            // label is relevant in both transition_systems
            pair_weight =
                min(pair_weight, max(label_ranks1[i], label_ranks2[i]));
        }
        scores.push_back(pair_weight);
    }
    return scores;
}

string MergeScoringFunctionDFP::name() const
{
    return "dfp";
}

class MergeScoringFunctionDFPFeature
    : public plugins::
          TypedFeature<MergeScoringFunction, MergeScoringFunctionDFP> {
public:
    MergeScoringFunctionDFPFeature()
        : TypedFeature("pdfp")
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

    shared_ptr<MergeScoringFunctionDFP>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return make_shared<MergeScoringFunctionDFP>();
    }
};

static plugins::FeaturePlugin<MergeScoringFunctionDFPFeature> _plugin;

} // namespace probfd::merge_and_shrink