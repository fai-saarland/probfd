#include "downward/cli/heuristics/merge_and_shrink_heuristic_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/merge_and_shrink/merge_and_shrink_algorithm_options.h"

#include "downward/cli/heuristics/heuristic_options.h"

#include "downward/merge_and_shrink/merge_and_shrink_heuristic.h"

#include "downward/utils/markup.h"

#include "downward/task_dependent_factory.h"
#include "downward/task_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::utils;

using namespace language::plugins;

using downward::cli::add_heuristic_options_to_feature;

using downward::cli::merge_and_shrink::
    add_transition_system_size_limit_options_to_feature;
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
    utils::FSeconds main_loop_max_time;

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
        utils::FSeconds main_loop_max_time)
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

    unique_ptr<Evaluator> create_object(const SharedAbstractTask& task) override
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

shared_ptr<TaskDependentFactory<Evaluator>> create_merge_and_shrink_heuristic(
    shared_ptr<TaskTransformation> transformation,
    bool cache_estimates,
    string description,
    Verbosity verbosity,
    shared_ptr<MergeStrategyFactory> merge_strategy,
    shared_ptr<ShrinkStrategy> shrink_strategy,
    shared_ptr<LabelReduction> label_reduction,
    bool prune_unreachable_states,
    bool prune_irrelevant_states,
    int max_states,
    int max_states_before_merge,
    int threshold_before_merge,
    FSeconds main_loop_max_time)
{

    handle_shrink_limit_options_defaults(
        max_states,
        max_states_before_merge,
        threshold_before_merge);

    return make_shared_from_arg_tuples<MergeAndShrinkHeuristicFactory>(
        std::move(transformation),
        cache_estimates,
        std::move(description),
        verbosity,
        std::move(merge_strategy),
        std::move(shrink_strategy),
        std::move(label_reduction),
        prune_unreachable_states,
        prune_irrelevant_states,
        max_states,
        max_states_before_merge,
        threshold_before_merge,
        main_loop_max_time);
}

shared_ptr<TaskDependentFactory<Evaluator>>
create_merge_and_shrink_heuristic_no_lr(
    shared_ptr<TaskTransformation> transformation,
    bool cache_estimates,
    string description,
    Verbosity verbosity,
    shared_ptr<MergeStrategyFactory> merge_strategy,
    shared_ptr<ShrinkStrategy> shrink_strategy,
    bool prune_unreachable_states,
    bool prune_irrelevant_states,
    int max_states,
    int max_states_before_merge,
    int threshold_before_merge,
    FSeconds main_loop_max_time)
{
    return create_merge_and_shrink_heuristic(
        std::move(transformation),
        cache_estimates,
        std::move(description),
        verbosity,
        std::move(merge_strategy),
        std::move(shrink_strategy),
        nullptr,
        prune_unreachable_states,
        prune_irrelevant_states,
        max_states,
        max_states_before_merge,
        threshold_before_merge,
        main_loop_max_time);
}

InternalFunctionDefinitionBase&
add_merge_and_shrink_heuristic_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "merge_and_shrink",
        create_merge_and_shrink_heuristic);

    f.document_title("Merge-and-shrink heuristic");
    f.document_synopsis(
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

    f.document_note(
        "Note",
        "Conditional effects are supported directly. Note, however, that "
        "for tasks that are not factored (in the sense of the JACM 2014 "
        "merge-and-shrink paper), the atomic transition systems on which "
        "merge-and-shrink heuristics are based are nondeterministic, "
        "which can lead to poor heuristics even when only perfect "
        "shrinking "
        "is performed.");
    f.document_note(
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
    f.document_note(
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
    f.document_note(
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

    f.document_language_support("action costs", "supported");
    f.document_language_support(
        "conditional effects",
        "supported (but see note)");
    f.document_language_support("axioms", "not supported");

    f.document_property("admissible", "yes (but see note)");
    f.document_property("consistent", "yes (but see note)");
    f.document_property("safe", "yes");
    f.document_property("preferred operators", "no");

    // Merge strategy option.
    f.make_required_argument(
        0,
        "merge_strategy",
        "See detailed documentation for merge strategies. "
        "We currently recommend SCC-DFP, which can be achieved using "
        "{{{merge_strategy=merge_sccs(order_of_sccs=topological,merge_"
        "selector="
        "score_based_filtering(scoring_functions=[goal_relevance,dfp,total_"
        "order"
        "]))}}}");

    // Shrink strategy option.
    f.make_required_argument(
        1,
        "shrink_strategy",
        "See detailed documentation for shrink strategies. "
        "We currently recommend non-greedy shrink_bisimulation, which can "
        "be "
        "achieved using "
        "{{{shrink_strategy=shrink_bisimulation(greedy=false)}}}");

    // Label reduction option.
    f.make_required_argument(
        2,
        "label_reduction",
        "See detailed documentation for labels. "
        "Note the interaction with shrink strategies.");

    // Pruning options.
    f.make_optional_argument_with_default(
        3,
        "prune_unreachable_states",
        "true",
        "If true, prune abstract states unreachable from the initial "
        "state.");

    f.make_optional_argument_with_default(
        4,
        "prune_irrelevant_states",
        "true",
        "If true, prune abstract states from which no goal state can be "
        "reached.");

    const auto n = add_transition_system_size_limit_options_to_feature(f, 5);

    f.make_optional_argument_with_default(
        n + 5,
        "main_loop_max_time",
        "seconds_max()",
        "A limit in seconds on the runtime of the main loop of the "
        "algorithm. "
        "If the limit is exceeded, the algorithm terminates, potentially "
        "returning a factored transition system with several factors. Also "
        "note that the time limit is only checked between transformations "
        "of the main loop, but not during, so it can be exceeded if a "
        "transformation is runtime-intense.");

    add_heuristic_options_to_feature(f, "merge_and_shrink", n + 6);

    return f;
}

InternalFunctionDefinitionBase&
add_merge_and_shrink_heuristic_no_lr_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "merge_and_shrink_no_lr",
        create_merge_and_shrink_heuristic_no_lr);

    f.document_title("Merge-and-shrink heuristic");
    f.document_synopsis(
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

    f.document_note(
        "Note",
        "Conditional effects are supported directly. Note, however, that "
        "for tasks that are not factored (in the sense of the JACM 2014 "
        "merge-and-shrink paper), the atomic transition systems on which "
        "merge-and-shrink heuristics are based are nondeterministic, "
        "which can lead to poor heuristics even when only perfect "
        "shrinking "
        "is performed.");
    f.document_note(
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
    f.document_note(
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
    f.document_note(
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

    f.document_language_support("action costs", "supported");
    f.document_language_support(
        "conditional effects",
        "supported (but see note)");
    f.document_language_support("axioms", "not supported");

    f.document_property("admissible", "yes (but see note)");
    f.document_property("consistent", "yes (but see note)");
    f.document_property("safe", "yes");
    f.document_property("preferred operators", "no");

    // Merge strategy option.
    f.make_required_argument(
        0,
        "merge_strategy",
        "See detailed documentation for merge strategies. "
        "We currently recommend SCC-DFP, which can be achieved using "
        "{{{merge_strategy=merge_sccs(order_of_sccs=topological,merge_"
        "selector="
        "score_based_filtering(scoring_functions=[goal_relevance,dfp,total_"
        "order"
        "]))}}}");

    // Shrink strategy option.
    f.make_required_argument(
        1,
        "shrink_strategy",
        "See detailed documentation for shrink strategies. "
        "We currently recommend non-greedy shrink_bisimulation, which can "
        "be "
        "achieved using "
        "{{{shrink_strategy=shrink_bisimulation(greedy=false)}}}");

    // Pruning options.
    f.make_optional_argument_with_default(
        2,
        "prune_unreachable_states",
        "true",
        "If true, prune abstract states unreachable from the initial "
        "state.");

    f.make_optional_argument_with_default(
        3,
        "prune_irrelevant_states",
        "true",
        "If true, prune abstract states from which no goal state can be "
        "reached.");

    const auto n = add_transition_system_size_limit_options_to_feature(f, 4);

    f.make_optional_argument_with_default(
        n + 4,
        "main_loop_max_time",
        "seconds_max()",
        "A limit in seconds on the runtime of the main loop of the "
        "algorithm. "
        "If the limit is exceeded, the algorithm terminates, potentially "
        "returning a factored transition system with several factors. Also "
        "note that the time limit is only checked between transformations "
        "of the main loop, but not during, so it can be exceeded if a "
        "transformation is runtime-intense.");

    add_heuristic_options_to_feature(f, "merge_and_shrink", n + 5);

    return f;
}

} // namespace

namespace downward::cli::heuristics {

void add_merge_and_shrink_heuristic_feature(Namespace& nspace)
{
    add_merge_and_shrink_heuristic_to_namespace(nspace);
    add_merge_and_shrink_heuristic_no_lr_to_namespace(nspace);
}

} // namespace downward::cli::heuristics
