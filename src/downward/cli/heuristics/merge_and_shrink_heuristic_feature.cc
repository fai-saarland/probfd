#include "downward/cli/plugins/plugin.h"

#include "downward/cli/merge_and_shrink/merge_and_shrink_algorithm_options.h"

#include "downward/cli/heuristic_options.h"

#include "downward/merge_and_shrink/merge_and_shrink_heuristic.h"

#include "downward/utils/markup.h"

#include "downward/task_dependent_factory.h"
#include "downward/task_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::add_heuristic_options_to_feature;
using downward::cli::get_heuristic_arguments_from_options;

using downward::cli::merge_and_shrink::
    add_merge_and_shrink_algorithm_options_to_feature;
using downward::cli::merge_and_shrink::
    get_merge_and_shrink_algorithm_arguments_from_options;

using downward::cli::merge_and_shrink::handle_shrink_limit_options_defaults;

using namespace downward::merge_and_shrink;

namespace {

class MergeAndShrinkHeuristicFactory : public TaskDependentFactory<Evaluator> {
    std::shared_ptr<TaskTransformation> transformation;
    bool cache_estimates;
    std::string description;
    utils::Verbosity verbosity;
    shared_ptr<MergeStrategyFactory> merge_strategy;
    shared_ptr<ShrinkStrategy> shrink_strategy;
    shared_ptr<LabelReduction> label_reduction;
    bool prune_unreachable_states;
    bool prune_irrelevant_states;
    int max_states;
    int max_states_before_merge;
    int threshold_before_merge;
    double main_loop_max_time;

public:
    MergeAndShrinkHeuristicFactory(
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        utils::Verbosity verbosity,
        shared_ptr<MergeStrategyFactory> merge_strategy,
        shared_ptr<ShrinkStrategy> shrink_strategy,
        shared_ptr<LabelReduction> label_reduction,
        bool prune_unreachable_states,
        bool prune_irrelevant_states,
        int max_states,
        int max_states_before_merge,
        int threshold_before_merge,
        double main_loop_max_time)
        : transformation(std::move(transformation))
        , cache_estimates(cache_estimates)
        , description(std::move(description))
        , verbosity(verbosity)
        , merge_strategy(std::move(merge_strategy))
        , shrink_strategy(std::move(shrink_strategy))
        , label_reduction(std::move(label_reduction))
        , prune_unreachable_states(prune_unreachable_states)
        , prune_irrelevant_states(prune_irrelevant_states)
        , max_states(max_states)
        , max_states_before_merge(max_states_before_merge)
        , threshold_before_merge(threshold_before_merge)
        , main_loop_max_time(main_loop_max_time)
    {
    }

    unique_ptr<Evaluator>
    create_object(const std::shared_ptr<AbstractTask>& task) override
    {
        auto transformation_result = transformation->transform(task);
        return std::make_unique<MergeAndShrinkHeuristic>(
            merge_strategy,
            shrink_strategy,
            label_reduction,
            prune_unreachable_states,
            prune_irrelevant_states,
            max_states,
            max_states_before_merge,
            threshold_before_merge,
            main_loop_max_time,
            task,
            std::move(transformation_result),
            cache_estimates,
            description,
            verbosity);
    }
};

class MergeAndShrinkHeuristicFeature
    : public TypedFeature<
          TaskDependentFactory<Evaluator>,
          MergeAndShrinkHeuristicFactory> {
public:
    MergeAndShrinkHeuristicFeature()
        : TypedFeature("merge_and_shrink")
    {
        document_title("Merge-and-shrink heuristic");
        document_synopsis(
            "This heuristic implements the algorithm described in the "
            "following "
            "paper:" +
            format_conference_reference(
                {"Silvan Sievers", "Martin Wehrle", "Malte Helmert"},
                "Generalized Label Reduction for Merge-and-Shrink Heuristics",
                "https://ai.dmi.unibas.ch/papers/sievers-et-al-aaai2014.pdf",
                "Proceedings of the 28th AAAI Conference on Artificial"
                " Intelligence (AAAI 2014)",
                "2358-2366",
                "AAAI Press",
                "2014") +
            "\n" +
            "For a more exhaustive description of merge-and-shrink, see the "
            "journal "
            "paper" +
            format_journal_reference(
                {"Silvan Sievers", "Malte Helmert"},
                "Merge-and-Shrink: A Compositional Theory of Transformations "
                "of Factored Transition Systems",
                "https://ai.dmi.unibas.ch/papers/sievers-helmert-jair2021.pdf",
                "Journal of Artificial Intelligence Research",
                "71",
                "781-883",
                "2021") +
            "\n" +
            "The following paper describes how to improve the DFP merge "
            "strategy "
            "with tie-breaking, and presents two new merge strategies "
            "(dyn-MIASM "
            "and SCC-DFP):" +
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
            "\n" +
            "Details of the algorithms and the implementation are described in "
            "the "
            "paper" +
            format_conference_reference(
                {"Silvan Sievers"},
                "Merge-and-Shrink Heuristics for Classical Planning: Efficient "
                "Implementation and Partial Abstractions",
                "https://ai.dmi.unibas.ch/papers/sievers-socs2018.pdf",
                "Proceedings of the 11th Annual Symposium on Combinatorial "
                "Search "
                "(SoCS 2018)",
                "90-98",
                "AAAI Press",
                "2018"));

        add_merge_and_shrink_algorithm_options_to_feature(*this);
        add_heuristic_options_to_feature(*this, "merge_and_shrink");

        document_note(
            "Note",
            "Conditional effects are supported directly. Note, however, that "
            "for tasks that are not factored (in the sense of the JACM 2014 "
            "merge-and-shrink paper), the atomic transition systems on which "
            "merge-and-shrink heuristics are based are nondeterministic, "
            "which can lead to poor heuristics even when only perfect "
            "shrinking "
            "is performed.");
        document_note(
            "Note",
            "When pruning unreachable states, admissibility and consistency is "
            "only guaranteed for reachable states and transitions between "
            "reachable states. While this does not impact regular A* search "
            "which "
            "will never encounter any unreachable state, it impacts techniques "
            "like symmetry-based pruning: a reachable state which is mapped to "
            "an "
            "unreachable symmetric state (which hence is pruned) would falsely "
            "be "
            "considered a dead-end and also be pruned, thus violating "
            "optimality "
            "of the search.");
        document_note(
            "Note",
            "When using a time limit on the main loop of the merge-and-shrink "
            "algorithm, the heuristic will compute the maximum over all "
            "heuristics "
            "induced by the remaining factors if terminating the "
            "merge-and-shrink "
            "algorithm early. Exception: if there is an unsolvable factor, it "
            "will "
            "be used as the exclusive heuristic since the problem is "
            "unsolvable.");
        document_note(
            "Note",
            "A currently recommended good configuration uses bisimulation "
            "based shrinking, the merge strategy SCC-DFP, and the appropriate "
            "label reduction setting (max_states has been altered to be "
            "between "
            "10k and 200k in the literature). As merge-and-shrink heuristics "
            "can be expensive to compute, we also recommend limiting time by "
            "setting {{{main_loop_max_time}}} to a finite value. A sensible "
            "value would be half of the time allocated for the planner.\n"
            "{{{\nmerge_and_shrink(shrink_strategy=shrink_bisimulation(greedy="
            "false),"
            "merge_strategy=merge_sccs(order_of_sccs=topological,merge_"
            "selector="
            "score_based_filtering(scoring_functions=[goal_relevance(),dfp(),"
            "total_order()])),label_reduction=exact(before_shrinking=true,"
            "before_merging=false),max_states=50k,threshold_before_merge=1)\n}}"
            "}\n");

        document_language_support("action costs", "supported");
        document_language_support(
            "conditional effects",
            "supported (but see note)");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes (but see note)");
        document_property("consistent", "yes (but see note)");
        document_property("safe", "yes");
        document_property("preferred operators", "no");
    }

    shared_ptr<MergeAndShrinkHeuristicFactory>
    create_component(const Options& opts, const Context& context) const override
    {
        Options options_copy(opts);
        handle_shrink_limit_options_defaults(options_copy, context);
        return make_shared_from_arg_tuples<MergeAndShrinkHeuristicFactory>(
            get_heuristic_arguments_from_options(options_copy),
            get_merge_and_shrink_algorithm_arguments_from_options(
                options_copy));
    }
};

FeaturePlugin<MergeAndShrinkHeuristicFeature> _plugin;

} // namespace
