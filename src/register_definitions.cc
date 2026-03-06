#include "register_definitions.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_function_definition.h"
#include "language/ast/namespace_declaration.h"

#include "downward/utils/string_literal.h"
#include "downward/utils/timer.h"

#include "downward/cli/cartesian_abstractions/subtask_generators_category.h"
#include "downward/cli/cartesian_abstractions/subtask_generators_features.h"

#include "downward/cli/evaluators/const_evaluator_feature.h"
#include "downward/cli/evaluators/evaluator_category.h"
#include "downward/cli/evaluators/g_evaluator_feature.h"
#include "downward/cli/evaluators/max_evaluator_feature.h"
#include "downward/cli/evaluators/pref_evaluator_feature.h"
#include "downward/cli/evaluators/subcategory.h"
#include "downward/cli/evaluators/sum_evaluator_feature.h"
#include "downward/cli/evaluators/weighted_evaluator_feature.h"

#include "downward/cli/heuristics/additive_cartesian_heuristic_feature.h"
#include "downward/cli/heuristics/additive_heuristic_feature.h"
#include "downward/cli/heuristics/blind_search_heuristic_feature.h"
#include "downward/cli/heuristics/canonical_pdbs_heuristic_feature.h"
#include "downward/cli/heuristics/cea_heuristic_feature.h"
#include "downward/cli/heuristics/cg_heuristic_feature.h"
#include "downward/cli/heuristics/diverse_potential_heuristics_feature.h"
#include "downward/cli/heuristics/ff_heuristic_feature.h"
#include "downward/cli/heuristics/goal_count_heuristic_feature.h"
#include "downward/cli/heuristics/hm_heuristic_feature.h"
#include "downward/cli/heuristics/ipdbs_heuristic_feature.h"
#include "downward/cli/heuristics/landmark_cost_partitioning_heuristic_feature.h"
#include "downward/cli/heuristics/landmark_sum_heuristic_feature.h"
#include "downward/cli/heuristics/lm_cut_heuristic_feature.h"
#include "downward/cli/heuristics/max_heuristic_feature.h"
#include "downward/cli/heuristics/merge_and_shrink_heuristic_feature.h"
#include "downward/cli/heuristics/operator_counting_heuristic_feature.h"
#include "downward/cli/heuristics/pdb_heuristic_feature.h"
#include "downward/cli/heuristics/sample_based_potential_heuristics_feature.h"
#include "downward/cli/heuristics/single_potential_heuristics_feature.h"
#include "downward/cli/heuristics/zero_one_pdbs_heuristic_feature.h"

#include "downward/cli/landmarks/landmark_factory_category.h"
#include "downward/cli/landmarks/landmark_factory_h_m_feature.h"
#include "downward/cli/landmarks/landmark_factory_merged_feature.h"
#include "downward/cli/landmarks/landmark_factory_reasonable_orders_hps_feature.h"
#include "downward/cli/landmarks/landmark_factory_rpg_exhaust_feature.h"
#include "downward/cli/landmarks/landmark_factory_rpg_sasp_feature.h"
#include "downward/cli/landmarks/landmark_factory_zhu_givan_feature.h"

#include "downward/cli/lp/lp_solver_enum.h"

#include "downward/cli/merge_and_shrink/label_reduction_feature.h"
#include "downward/cli/merge_and_shrink/merge_scoring_function_category.h"
#include "downward/cli/merge_and_shrink/merge_scoring_function_dfp_feature.h"
#include "downward/cli/merge_and_shrink/merge_scoring_function_goal_relevance_feature.h"
#include "downward/cli/merge_and_shrink/merge_scoring_function_miasm_feature.h"
#include "downward/cli/merge_and_shrink/merge_scoring_function_single_random_feature.h"
#include "downward/cli/merge_and_shrink/merge_scoring_function_total_order_feature.h"
#include "downward/cli/merge_and_shrink/merge_selector_category.h"
#include "downward/cli/merge_and_shrink/merge_selector_score_based_filtering_feature.h"
#include "downward/cli/merge_and_shrink/merge_strategy_factory_category.h"
#include "downward/cli/merge_and_shrink/merge_strategy_factory_precomputed_feature.h"
#include "downward/cli/merge_and_shrink/merge_strategy_factory_sccs_feature.h"
#include "downward/cli/merge_and_shrink/merge_strategy_factory_stateless_feature.h"
#include "downward/cli/merge_and_shrink/merge_tree_factory_category.h"
#include "downward/cli/merge_and_shrink/merge_tree_factory_linear_feature.h"
#include "downward/cli/merge_and_shrink/shrink_bisimulation_feature.h"
#include "downward/cli/merge_and_shrink/shrink_fh_feature.h"
#include "downward/cli/merge_and_shrink/shrink_random_feature.h"
#include "downward/cli/merge_and_shrink/shrink_strategy_category.h"

#include "downward/cli/mutexes/from_file_mutex_factory_feature.h"
#include "downward/cli/mutexes/mutex_factory_category.h"

#include "downward/cli/open_lists/alternation_open_list_feature.h"
#include "downward/cli/open_lists/best_first_open_list_feature.h"
#include "downward/cli/open_lists/epsilon_greedy_open_list_feature.h"
#include "downward/cli/open_lists/open_list_factory_category.h"
#include "downward/cli/open_lists/pareto_open_list_feature.h"
#include "downward/cli/open_lists/tiebreaking_open_list_feature.h"
#include "downward/cli/open_lists/type_based_open_list_feature.h"

#include "downward/cli/operator_counting/constraint_generator_category.h"
#include "downward/cli/operator_counting/delete_relaxation_if_constraints_feature.h"
#include "downward/cli/operator_counting/delete_relaxation_rr_constraints_feature.h"
#include "downward/cli/operator_counting/lm_cut_constraints_feature.h"
#include "downward/cli/operator_counting/pho_constraints_feature.h"
#include "downward/cli/operator_counting/state_equation_constraints_feature.h"

#include "downward/cli/pdbs/pattern_collection_generator_category.h"
#include "downward/cli/pdbs/pattern_collection_generator_combo_feature.h"
#include "downward/cli/pdbs/pattern_collection_generator_disjoint_cegar_feature.h"
#include "downward/cli/pdbs/pattern_collection_generator_genetic_feature.h"
#include "downward/cli/pdbs/pattern_collection_generator_hillclimbing_feature.h"
#include "downward/cli/pdbs/pattern_collection_generator_manual_feature.h"
#include "downward/cli/pdbs/pattern_collection_generator_multiple_cegar_feature.h"
#include "downward/cli/pdbs/pattern_collection_generator_multiple_random_feature.h"
#include "downward/cli/pdbs/pattern_collection_generator_systematic_feature.h"
#include "downward/cli/pdbs/pattern_generator_category.h"
#include "downward/cli/pdbs/pattern_generator_cegar_feature.h"
#include "downward/cli/pdbs/pattern_generator_greedy_feature.h"
#include "downward/cli/pdbs/pattern_generator_manual_feature.h"
#include "downward/cli/pdbs/pattern_generator_random_feature.h"
#include "downward/cli/pdbs/subcategory.h"

#include "downward/cli/potentials/subcategory.h"

#include "downward/cli/pruning/limited_pruning_feature.h"
#include "downward/cli/pruning/null_pruning_method_feature.h"
#include "downward/cli/pruning/pruning_method_category.h"
#include "downward/cli/pruning/stubborn_sets_atom_centric_feature.h"
#include "downward/cli/pruning/stubborn_sets_ec_feature.h"
#include "downward/cli/pruning/stubborn_sets_simple_feature.h"

#include "downward/cli/search_algorithms/astar_feature.h"
#include "downward/cli/search_algorithms/eager_feature.h"
#include "downward/cli/search_algorithms/eager_greedy_feature.h"
#include "downward/cli/search_algorithms/eager_wastar_feature.h"
#include "downward/cli/search_algorithms/enforced_hill_climbing_search_feature.h"
#include "downward/cli/search_algorithms/iterated_search_feature.h"
#include "downward/cli/search_algorithms/lazy_feature.h"
#include "downward/cli/search_algorithms/lazy_greedy_feature.h"
#include "downward/cli/search_algorithms/lazy_wastar_feature.h"
#include "downward/cli/search_algorithms/search_algorithm_factory_category.h"

#include "downward/cli/tasks/cost_task_transformation_feature.h"
#include "downward/cli/tasks/identity_task_transformation_feature.h"
#include "downward/cli/tasks/task_transformation_category.h"

#include "downward/cli/utils/verbosity_enum.h"

#include "downward/cli/operator_cost_category.h"
#include "downward/cli/utils/rng_options.h"
#include "downward/utils/math.h"
#include "language/ast/internal_type_declaration.h"

#include "probfd/cli/cartesian_abstractions/adaptive_flaw_generator.h"
#include "probfd/cli/cartesian_abstractions/flaw_generator_category.h"
#include "probfd/cli/cartesian_abstractions/policy_based_flaw_generator.h"
#include "probfd/cli/cartesian_abstractions/split_selector.h"
#include "probfd/cli/cartesian_abstractions/subtask_generators.h"
#include "probfd/cli/cartesian_abstractions/trace_based_flaw_generator.h"

#include "probfd/cli/heuristics/additive_cartesian_heuristic.h"
#include "probfd/cli/heuristics/constant_heuristic.h"
#include "probfd/cli/heuristics/dead_end_pruning_heuristic.h"
#include "probfd/cli/heuristics/determinization_cost_heuristic.h"
#include "probfd/cli/heuristics/gzocp_heuristic.h"
#include "probfd/cli/heuristics/merge_and_shrink_heuristic.h"
#include "probfd/cli/heuristics/probability_aware_pdb_heuristic.h"
#include "probfd/cli/heuristics/scp_heuristic.h"
#include "probfd/cli/heuristics/task_heuristic_factory_category.h"
#include "probfd/cli/heuristics/ucp_heuristic.h"

#include "probfd/cli/merge_and_shrink/label_reduction_feature.h"
#include "probfd/cli/merge_and_shrink/merge_scoring_function_category.h"
#include "probfd/cli/merge_and_shrink/merge_scoring_function_dfp.h"
#include "probfd/cli/merge_and_shrink/merge_scoring_function_goal_relevance.h"
#include "probfd/cli/merge_and_shrink/merge_scoring_function_miasm.h"
#include "probfd/cli/merge_and_shrink/merge_scoring_function_single_random.h"
#include "probfd/cli/merge_and_shrink/merge_scoring_function_total_order.h"
#include "probfd/cli/merge_and_shrink/merge_selector_category.h"
#include "probfd/cli/merge_and_shrink/merge_selector_score_based_filtering.h"
#include "probfd/cli/merge_and_shrink/merge_strategy_factory_category.h"
#include "probfd/cli/merge_and_shrink/merge_strategy_factory_precomputed.h"
#include "probfd/cli/merge_and_shrink/merge_strategy_factory_sccs.h"
#include "probfd/cli/merge_and_shrink/merge_strategy_factory_stateless.h"
#include "probfd/cli/merge_and_shrink/merge_tree_factory_category.h"
#include "probfd/cli/merge_and_shrink/merge_tree_factory_linear.h"
#include "probfd/cli/merge_and_shrink/prune_strategy_alive.h"
#include "probfd/cli/merge_and_shrink/prune_strategy_category.h"
#include "probfd/cli/merge_and_shrink/prune_strategy_identity.h"
#include "probfd/cli/merge_and_shrink/prune_strategy_solvable.h"
#include "probfd/cli/merge_and_shrink/shrink_strategy_bisimulation.h"
#include "probfd/cli/merge_and_shrink/shrink_strategy_category.h"
#include "probfd/cli/merge_and_shrink/shrink_strategy_equal_distance.h"
#include "probfd/cli/merge_and_shrink/shrink_strategy_probabilistic_bisimulation.h"
#include "probfd/cli/merge_and_shrink/shrink_strategy_random.h"

#include "probfd/cli/occupation_measures/constraint_generator_factory_category.h"
#include "probfd/cli/occupation_measures/subcategory.h"

#include "probfd/cli/open_lists/subcategory.h"

#include "probfd/cli/pdbs/cegar/bfs_flaw_finder.h"
#include "probfd/cli/pdbs/cegar/flaw_finding_strategy_category.h"
#include "probfd/cli/pdbs/cegar/pucs_flaw_finder.h"
#include "probfd/cli/pdbs/cegar/sampling_flaw_finder.h"
#include "probfd/cli/pdbs/fully_additive_finder_factory.h"
#include "probfd/cli/pdbs/max_orthogonal_finder_factory.h"
#include "probfd/cli/pdbs/pattern_collection_generator_category.h"
#include "probfd/cli/pdbs/pattern_collection_generator_classical.h"
#include "probfd/cli/pdbs/pattern_collection_generator_disjoint_cegar.h"
#include "probfd/cli/pdbs/pattern_collection_generator_hillclimbing.h"
#include "probfd/cli/pdbs/pattern_collection_generator_multiple_cegar.h"
#include "probfd/cli/pdbs/pattern_collection_generator_systematic.h"
#include "probfd/cli/pdbs/pattern_generator_category.h"
#include "probfd/cli/pdbs/subcollection_finder_factory_category.h"
#include "probfd/cli/pdbs/trivial_finder_factory.h"

#include "probfd/cli/policy_pickers/subcategory.h"

#include "probfd/cli/solvers/acyclic_vi.h"
#include "probfd/cli/solvers/aostar.h"
#include "probfd/cli/solvers/bisimulation_vi.h"
#include "probfd/cli/solvers/depth_first_heuristic_search.h"
#include "probfd/cli/solvers/exhaustive_ao.h"
#include "probfd/cli/solvers/exhaustive_dfs.h"
#include "probfd/cli/solvers/i2dual.h"
#include "probfd/cli/solvers/idual.h"
#include "probfd/cli/solvers/interval_iteration.h"
#include "probfd/cli/solvers/lrtdp.h"
#include "probfd/cli/solvers/statistical_mdp_algorithm_factory_category.h"
#include "probfd/cli/solvers/ta_depth_first_heuristic_search.h"
#include "probfd/cli/solvers/ta_lrtdp.h"
#include "probfd/cli/solvers/ta_topological_vi.h"
#include "probfd/cli/solvers/task_solver_factory_category.h"
#include "probfd/cli/solvers/topological_vi.h"

#include "probfd/cli/successor_samplers/subcategory.h"

#include "probfd/cli/transition_sorters/subcategory.h"

#include "probfd/cli/task_state_space_factory_category.h"
#include "probfd/cli/task_state_space_factory_features.h"

using namespace language;
using namespace language::parser;

namespace {

int make_infinite_value()
{
    return std::numeric_limits<int>::max();
}

template <typename T, T F>
T scale(T v)
{
    if constexpr (std::same_as<T, int>) {
        if (!downward::utils::is_product_within_limits(
                v,
                F,
                std::numeric_limits<T>::min(),
                std::numeric_limits<T>::max() - 1)) {
            throw std::overflow_error("Integer would be out of range!");
        }
    }
    return F * v;
}

template <typename R, typename T>
    requires std::constructible_from<R, T>
R cast(T v)
{
    return static_cast<R>(v);
}

template <typename T>
T max_duration()
{
    return T::max();
}

void add_infinity_feature_to_namespace(NamespaceLevelDeclarationList& list)
{
    insert_function_definition<make_infinite_value>(list, "infinity");
}

template <typename T, T F>
void add_scale_literal_feature_to_namespace(
    NamespaceLevelDeclarationList& list,
    std::string name)
{
    insert_function_definition<scale<T, F>>(list, std::move(name));
}

template <typename R, typename T>
void add_cast_from_literal_to_namespace(
    NamespaceLevelDeclarationList& list,
    std::string name)
{
    insert_function_definition<cast<R, T>>(list, std::move(name));
}

template <typename T>
void add_infinite_duration_feature_to_namespace(
    NamespaceLevelDeclarationList& list,
    std::string name)
{
    auto& f =
        insert_function_definition<max_duration<T>>(list, std::move(name));
    f.document_synopsis("Returns a maximum / infinite duration.");
}

} // namespace

static void register_language_definitions(CompilationContext& ccontext)
{
    using namespace downward::utils::string_literals;

    // Infinity
    add_infinity_feature_to_namespace(ccontext);

    // Generic literal suffixes
    add_scale_literal_feature_to_namespace<int, 1'000>(
        ccontext,
        "__operator_int_k__");
    add_scale_literal_feature_to_namespace<int, 1'000'000>(
        ccontext,
        "__operator_int_m__");
    add_scale_literal_feature_to_namespace<int, 1'000'000'000>(
        ccontext,
        "__operator_int_g__");

    add_scale_literal_feature_to_namespace<double, 1'000.>(
        ccontext,
        "__operator_float_k__");
    add_scale_literal_feature_to_namespace<double, 1'000'000.>(
        ccontext,
        "__operator_float_m__");
    add_scale_literal_feature_to_namespace<double, 1'000'000'000.>(
        ccontext,
        "__operator_float_g__");
}

static void register_fast_downward_types(CompilationContext& ccontext)
{
    using namespace downward::cli;

    // Duration types
    insert_type_declaration<downward::utils::FNanoSeconds>(
        ccontext,
        "nanoseconds",
        "Type representing a nanosecond.");
    insert_type_declaration<downward::utils::FMicroSeconds>(
        ccontext,
        "microseconds",
        "Type representing a microsecond.");
    insert_type_declaration<downward::utils::FMilliSeconds>(
        ccontext,
        "milliseconds",
        "Type representing a millisecond.");
    insert_type_declaration<downward::utils::FSeconds>(
        ccontext,
        "seconds",
        "Type representing a second.");
    insert_type_declaration<downward::utils::FMinutes>(
        ccontext,
        "minutes",
        "Type representing a minute.");
    insert_type_declaration<downward::utils::FHours>(
        ccontext,
        "hours",
        "Type representing an hour.");

        // Duration literals
    add_cast_from_literal_to_namespace<downward::utils::FNanoSeconds, int>(
        ccontext,
        "__operator_int_ns__");
    add_cast_from_literal_to_namespace<downward::utils::FMicroSeconds, int>(
        ccontext,
        "__operator_int_us__");
    add_cast_from_literal_to_namespace<downward::utils::FMilliSeconds, int>(
        ccontext,
        "__operator_int_ms__");
    add_cast_from_literal_to_namespace<downward::utils::FSeconds, int>(
        ccontext,
        "__operator_int_s__");
    add_cast_from_literal_to_namespace<downward::utils::FMinutes, int>(
        ccontext,
        "__operator_int_min__");
    add_cast_from_literal_to_namespace<downward::utils::FHours, int>(
        ccontext,
        "__operator_int_h__");

    add_cast_from_literal_to_namespace<downward::utils::FNanoSeconds, double>(
        ccontext,
        "__operator_float_ns__");
    add_cast_from_literal_to_namespace<downward::utils::FMicroSeconds, double>(
        ccontext,
        "__operator_float_us__");
    add_cast_from_literal_to_namespace<downward::utils::FMilliSeconds, double>(
        ccontext,
        "__operator_float_ms__");
    add_cast_from_literal_to_namespace<downward::utils::FSeconds, double>(
        ccontext,
        "__operator_float_s__");
    add_cast_from_literal_to_namespace<downward::utils::FMinutes, double>(
        ccontext,
        "__operator_float_min__");
    add_cast_from_literal_to_namespace<downward::utils::FHours, double>(
        ccontext,
        "__operator_float_h__");

    // Infinite durations
    add_infinite_duration_feature_to_namespace<downward::utils::FNanoSeconds>(
        ccontext,
        "nanoseconds_max");
    add_infinite_duration_feature_to_namespace<downward::utils::FMicroSeconds>(
        ccontext,
        "microseconds_max");
    add_infinite_duration_feature_to_namespace<downward::utils::FMilliSeconds>(
        ccontext,
        "milliseconds_max");
    add_infinite_duration_feature_to_namespace<downward::utils::FSeconds>(
        ccontext,
        "seconds_max");
    add_infinite_duration_feature_to_namespace<downward::utils::FMinutes>(
        ccontext,
        "minutes_max");
    add_infinite_duration_feature_to_namespace<downward::utils::FHours>(
        ccontext,
        "hours_max");

    // Cartesian abstractions
    cartesian_abstractions::add_subtask_generator_category(ccontext);

    // Evaluators
    evaluators::add_evaluator_category(ccontext);

    // Heuristics
    heuristics::add_additive_cartesian_heuristic_categories(ccontext);
    heuristics::add_landmark_cost_partitioning_heuristic_categories(ccontext);

    // Landmarks
    landmarks::add_landmark_factory_category(ccontext);

    // LP
    lp::add_lp_solver_enum(ccontext);

    // Merge-and-shrink
    merge_and_shrink::add_label_reduction_category(ccontext);
    merge_and_shrink::add_merge_scoring_function_category(ccontext);
    merge_and_shrink::add_merge_strategy_factory_category(ccontext);
    merge_and_shrink::add_merge_selector_category(ccontext);
    merge_and_shrink::add_merge_tree_factory_category(ccontext);
    merge_and_shrink::add_shrink_strategy_category(ccontext);

    // Mutexes
    mutexes::add_mutex_factory_category(ccontext);

    // Open Lists
    open_lists::add_open_list_factory_category(ccontext);

    // Operator Counting
    operator_counting::add_constraint_generator_category(ccontext);

    // PDBs
    pdbs::add_pattern_collection_generator_category(ccontext);
    pdbs::add_pattern_generator_category(ccontext);

    // Pruning
    pruning::add_pruning_method_category(ccontext);

    // Search Algorithms
    search_algorithms::add_search_algorithm_factory_category(ccontext);

    // Task Transformations
    tasks::add_task_transformation_category(ccontext);

    // Utils
    downward::cli::utils::add_rng_type(ccontext);
    downward::cli::utils::add_verbosity_enum(ccontext);

    // Operator Cost Enum
    add_operator_cost_category(ccontext);
}

static void register_fast_downward_definitions(CompilationContext& ccontext)
{
    using namespace downward::cli;

    // Cartesian abstractions
    cartesian_abstractions::add_subtask_generators_features(ccontext);

    // Evaluators
    {
        documentation::DocumentationTopic& subcategory =
            evaluators::add_evaluator_subcategory(ccontext);

        subcategory.add_declaration(
            evaluators::add_const_evaluator_feature(ccontext));
        subcategory.add_declaration(
            evaluators::add_g_evaluator_feature(ccontext));
        subcategory.add_declaration(
            evaluators::add_max_evaluator_feature(ccontext));
        subcategory.add_declaration(
            evaluators::add_pref_evaluator_feature(ccontext));
        subcategory.add_declaration(
            evaluators::add_sum_evaluator_feature(ccontext));
        subcategory.add_declaration(
            evaluators::add_weighted_evaluator_feature(ccontext));
    }

    // Heuristics
    heuristics::add_additive_cartesian_heuristic_feature(ccontext);
    heuristics::add_additive_heuristic_feature(ccontext);
    heuristics::add_blind_heuristic_feature(ccontext);
    heuristics::add_canonical_pdbs_heuristic_feature(ccontext);
    heuristics::add_cg_heuristic_feature(ccontext);
    heuristics::add_cea_heuristic_feature(ccontext);

    {
        documentation::DocumentationTopic& subcategory =
            potentials::add_potential_heuristics_subcategory(ccontext);
        subcategory.add_declaration(
            heuristics::add_diverse_potential_heuristics_feature(ccontext));
        subcategory.add_declaration(
            heuristics::add_sample_based_potential_heuristics_feature(
                ccontext));
        subcategory.add_declaration(
            heuristics::add_initial_state_potential_heuristic_feature(
                ccontext));
        subcategory.add_declaration(
            heuristics::add_all_states_potential_heuristic_feature(ccontext));
    }

    heuristics::add_ff_heuristic_features(ccontext);
    heuristics::add_goal_count_heuristic_features(ccontext);
    heuristics::add_hm_heuristic_features(ccontext);

    {
        documentation::DocumentationTopic& subcategory =
            pdbs::add_pdb_heuristic_subcategory(ccontext);
        subcategory.add_declaration(
            heuristics::add_pdb_heuristic_feature(ccontext));
        subcategory.add_declaration(
            heuristics::add_ipdbs_heuristic_features(ccontext));
        subcategory.add_declaration(
            heuristics::add_zero_one_pdbs_heuristic_feature(ccontext));
    }

    heuristics::add_landmark_cost_partitioning_heuristic_feature(ccontext);
    heuristics::add_landmark_cut_heuristic_feature(ccontext);
    heuristics::add_max_heuristic_feature(ccontext);
    heuristics::add_landmark_sum_heuristic_feature(ccontext);
    heuristics::add_merge_and_shrink_heuristic_feature(ccontext);
    heuristics::add_operator_counting_heuristic_feature(ccontext);

    // Landmarks
    landmarks::add_landmark_factory_hm_feature(ccontext);
    landmarks::add_landmark_factory_merged_feature(ccontext);
    landmarks::add_landmark_factory_reasonable_orders_hps_feature(ccontext);
    landmarks::add_landmark_factory_rpg_exhaust_feature(ccontext);
    landmarks::add_landmark_factory_rpg_sasp_feature(ccontext);
    landmarks::add_landmark_factory_zhu_givan_feature(ccontext);

    // Merge-and-shrink
    merge_and_shrink::add_label_reduction_to_namespace(ccontext);
    merge_and_shrink::add_merge_scoring_function_dfp_feature(ccontext);
    merge_and_shrink::add_merge_scoring_function_goal_relevance_feature(
        ccontext);
    merge_and_shrink::add_merge_scoring_function_miasm_feature(ccontext);
    merge_and_shrink::add_merge_scoring_function_total_order_feature(ccontext);
    merge_and_shrink::add_merge_scoring_function_single_random_feature(
        ccontext);
    merge_and_shrink::add_merge_selector_score_based_filtering_feature(
        ccontext);
    merge_and_shrink::add_merge_strategy_factory_precomputed_feature(ccontext);
    merge_and_shrink::add_merge_strategy_factory_sccs_feature(ccontext);
    merge_and_shrink::add_merge_strategy_factory_stateless_feature(ccontext);
    merge_and_shrink::add_merge_tree_factory_linear_feature(ccontext);
    merge_and_shrink::add_shrink_bisimulation_feature(ccontext);
    merge_and_shrink::add_shrink_fh_feature(ccontext);
    merge_and_shrink::add_shrink_random_feature(ccontext);

    // Mutexes
    mutexes::add_from_file_mutex_factory_feature(ccontext);

    // Open Lists
    open_lists::add_alternation_open_list_features(ccontext);
    open_lists::add_best_first_open_list_features(ccontext);
    open_lists::add_epsilon_greedy_open_list_features(ccontext);
    open_lists::add_pareto_open_list_features(ccontext);
    open_lists::add_tiebreaking_open_list_features(ccontext);
    open_lists::add_type_based_open_list_features(ccontext);

    // Operator Counting
    operator_counting::add_delete_relaxation_if_constraints_feature(ccontext);
    operator_counting::add_delete_relaxation_rr_constraints_feature(ccontext);
    operator_counting::add_lm_cut_constraints_feature(ccontext);
    operator_counting::add_pho_constraints_feature(ccontext);
    operator_counting::add_state_equation_constraints_feature(ccontext);

    // PDBs
    pdbs::add_pattern_collection_generator_combo_feature(ccontext);
    pdbs::add_pattern_collection_generator_disjoint_cegar_feature(ccontext);
    pdbs::add_pattern_collection_generator_genetic_feature(ccontext);
    pdbs::add_pattern_collection_generator_hillclimbing_feature(ccontext);
    pdbs::add_pattern_collection_generator_manual_feature(ccontext);
    pdbs::add_pattern_collection_generator_multiple_cegar_feature(ccontext);
    pdbs::add_pattern_collection_generator_multiple_random_feature(ccontext);
    pdbs::add_pattern_collection_generator_systematic_feature(ccontext);
    pdbs::add_pattern_generator_cegar_feature(ccontext);
    pdbs::add_pattern_generator_greedy_feature(ccontext);
    pdbs::add_pattern_generator_manual_feature(ccontext);
    pdbs::add_pattern_generator_random_feature(ccontext);

    // Pruning
    pruning::add_limited_pruning_feature(ccontext);
    pruning::add_null_pruning_method_feature(ccontext);
    pruning::add_stubborn_sets_atom_centric_feature(ccontext);
    pruning::add_stubborn_sets_ec_feature(ccontext);
    pruning::add_stubborn_sets_simple_feature(ccontext);

    // Search Algorithms
    search_algorithms::add_astar_feature(ccontext);
    search_algorithms::add_eager_feature(ccontext);
    search_algorithms::add_eager_greedy_feature(ccontext);
    search_algorithms::add_eager_wastar_feature(ccontext);
    search_algorithms::add_enforce_hill_climbing_search_feature(ccontext);
    search_algorithms::add_iterated_search_feature(ccontext);
    search_algorithms::add_lazy_feature(ccontext);
    search_algorithms::add_lazy_greedy_feature(ccontext);
    search_algorithms::add_lazy_wastar_feature(ccontext);

    // Task Transformations
    tasks::add_cost_task_transformation_features(ccontext);
    tasks::add_identity_task_transformation_features(ccontext);

    // Utils
    downward::cli::utils::add_default_rng_function(ccontext);
    downward::cli::utils::add_seeded_rng_function(ccontext);
}

static void register_probfd_types(CompilationContext& ccontext)
{
    using namespace probfd::cli;

    // Cartesian Abstractions
    cartesian_abstractions::add_flaw_generator_category(ccontext);
    cartesian_abstractions::add_subtask_generator_category(ccontext);
    cartesian_abstractions::add_split_selector_category(ccontext);

    // Heuristics
    heuristics::add_task_heuristic_factory_category(ccontext);

    // Merge-and-shrink
    merge_and_shrink::add_label_reduction_category(ccontext);
    merge_and_shrink::add_merge_scoring_function_category(ccontext);
    merge_and_shrink::add_merge_selector_category(ccontext);
    merge_and_shrink::add_merge_strategy_factory_category(ccontext);
    merge_and_shrink::add_merge_tree_factory_category(ccontext);
    merge_and_shrink::add_prune_strategy_category(ccontext);
    merge_and_shrink::add_shrink_strategy_category(ccontext);

    // Occupation Measures
    occupation_measures::add_constraint_generator_factory_category(ccontext);

    // Open Lists
    open_lists::add_open_list_categories(ccontext);

    // PDBs
    pdbs::add_pattern_collection_generator_category(ccontext);
    pdbs::add_pattern_generator_category(ccontext);
    pdbs::add_subcollection_finder_factory_category(ccontext);
    pdbs::cegar::add_flaw_finding_strategy_category(ccontext);

    // Policy Pickers
    policy_pickers::add_policy_picker_category(ccontext);

    // Solvers
    solvers::add_task_solver_factory_category(ccontext);
    solvers::add_statistical_mdp_algorithm_factory_category(ccontext);

    // Successor Samplers
    successor_samplers::add_successor_sampler_category(ccontext);

    // Transition Sorters
    transiton_sorters::add_transition_sorter_category(ccontext);

    // Task State Spaces
    add_task_state_space_factory_category(ccontext);
}

static void register_probfd_definitions(CompilationContext& ccontext)
{
    using namespace probfd::cli;

    // Cartesian Abstractions
    cartesian_abstractions::add_adaptive_flaw_generator_feature(ccontext);
    cartesian_abstractions::add_astar_flaw_generator_feature(ccontext);
    cartesian_abstractions::add_policy_based_flaw_generator_feature(ccontext);
    cartesian_abstractions::add_split_selector_features(ccontext);
    cartesian_abstractions::add_subtask_generator_features(ccontext);

    // Heuristics
    heuristics::add_additive_cartesian_heuristic_feature(ccontext);
    heuristics::add_dead_end_pruning_heuristic_feature(ccontext);
    heuristics::add_gzocp_heuristic_feature(ccontext);
    heuristics::add_merge_and_shrink_heuristic_feature(ccontext);
    heuristics::add_blind_heuristic_factory_feature(ccontext);
    heuristics::add_pdb_heuristic_feature(ccontext);
    heuristics::add_scp_heuristic_feature(ccontext);
    heuristics::add_ucp_heuristic_feature(ccontext);
    heuristics::add_determinization_cost_heuristic_feature(ccontext);

    // Merge-and-shrink
    merge_and_shrink::add_label_reduction_features(ccontext);
    merge_and_shrink::add_merge_scoring_function_dfp_feature(ccontext);
    merge_and_shrink::add_merge_scoring_function_goal_relevance_feature(
        ccontext);
    merge_and_shrink::add_merge_scoring_function_miasm_feature(ccontext);
    merge_and_shrink::add_merge_scoring_function_single_random_feature(
        ccontext);
    merge_and_shrink::add_merge_scoring_function_total_order_feature(ccontext);
    merge_and_shrink::add_merge_selector_score_based_filtering_feature(
        ccontext);
    merge_and_shrink::add_merge_strategy_factory_precomputed_feature(ccontext);
    merge_and_shrink::add_merge_strategy_factory_sccs_feature(ccontext);
    merge_and_shrink::add_merge_strategy_factory_stateless_feature(ccontext);
    merge_and_shrink::add_merge_tree_factory_linear_feature(ccontext);
    merge_and_shrink::add_prune_strategy_alive_feature(ccontext);
    merge_and_shrink::add_prune_strategy_identity_feature(ccontext);
    merge_and_shrink::add_prune_strategy_solvable_feature(ccontext);
    merge_and_shrink::add_shrink_strategy_bisimulation_feature(ccontext);
    merge_and_shrink::add_shrink_strategy_equal_distance_feature(ccontext);
    merge_and_shrink::add_shrink_strategy_random_feature(ccontext);
    merge_and_shrink::add_shrink_strategy_probabilistic_bisimulation_feature(
        ccontext);

    // Occupation Measures
    occupation_measures::add_occupation_measure_heuristics_features(ccontext);

    // Open Lists
    open_lists::add_open_list_features(ccontext);

    // PDBs
    pdbs::add_fully_additive_finder_factory_feature(ccontext);
    pdbs::add_max_orthogonal_finder_factory_feature(ccontext);
    pdbs::add_pattern_collection_generator_classical_feature(ccontext);
    pdbs::add_pattern_collection_generator_disjoint_cegar_feature(ccontext);
    pdbs::add_pattern_collection_generator_hillclimbing_feature(ccontext);
    pdbs::add_pattern_collection_generator_multiple_cegar_feature(ccontext);
    pdbs::add_pattern_collection_generator_systematic_feature(ccontext);
    pdbs::add_trivial_finder_factory_feature(ccontext);
    pdbs::cegar::add_bfs_flaw_finder_feature(ccontext);
    pdbs::cegar::add_pucs_flaw_finder_feature(ccontext);
    pdbs::cegar::add_sampling_flaw_finder_feature(ccontext);

    // Policy Pickers
    policy_pickers::add_policy_picker_features(ccontext);

    // Solvers
    solvers::add_acyclic_value_iteration_feature(ccontext);
    solvers::add_aostar_solver_features(ccontext);
    solvers::add_bisimulation_value_iteration_features(ccontext);
    solvers::add_depth_first_heuristic_search_features(ccontext);
    solvers::add_exhaustive_ao_solver_features(ccontext);
    solvers::add_exhaustive_dfs_feature(ccontext);
    solvers::add_i2dual_feature(ccontext);
    solvers::add_idual_feature(ccontext);
    solvers::add_interval_iteration_solver_feature(ccontext);
    solvers::add_lrtdp_features(ccontext);
    solvers::add_ta_depth_first_heuristic_search_feature(ccontext);
    solvers::add_ta_topological_value_iteration_feature(ccontext);
    solvers::add_ta_lrtdp_feature(ccontext);
    solvers::add_topological_value_iteration_feature(ccontext);

    // Successor Samplers
    successor_samplers::add_successor_sampler_features(ccontext);

    // Transition Sorters
    transiton_sorters::add_transition_sorter_features(ccontext);

    // Task State Spaces
    add_task_state_space_factory_features(ccontext);
}

namespace probfd {

void register_definitions(CompilationContext& ccontext)
{
    register_language_definitions(ccontext);

    register_fast_downward_types(ccontext);
    register_fast_downward_definitions(ccontext);

    register_probfd_types(ccontext);
    register_probfd_definitions(ccontext);
}

} // namespace probfd
