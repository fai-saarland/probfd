#include "register_definitions.h"

#include "downward/cli/plugins/plugin.h"

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
#include "downward/cli/plugins/registry.h"
#include "downward/utils/math.h"

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

using namespace downward::cli;

namespace {

class InfinityFeature : public plugins::TypedFeature<int> {
public:
    InfinityFeature()
        : TypedFeature("infinity", &InfinityFeature::func)
    {
    }

    static int func()
    {
        return std::numeric_limits<int>::max();
    }
};

template <typename T, T F, auto S>
class LiteralFeature : public plugins::TypedFeature<T, T> {
public:
    LiteralFeature()
        : LiteralFeature::TypedFeature(
              static_cast<std::string>(static_cast<std::string_view>(S)),
              &LiteralFeature::func)
    {
    }

    static T func(T v)
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
};

template <typename R, typename T, auto S>
class CastFromLiteralFeature : public plugins::TypedFeature<R, T> {
public:
    CastFromLiteralFeature()
        : CastFromLiteralFeature::TypedFeature(
              static_cast<std::string>(static_cast<std::string_view>(S)),
              &CastFromLiteralFeature::func)
    {
    }

    static R func(T v) { return R{v}; }
};

template <typename R, auto S>
class ConstructInfiniteFeature : public plugins::TypedFeature<R> {
public:
    ConstructInfiniteFeature()
        : ConstructInfiniteFeature::TypedFeature(
              static_cast<std::string>(static_cast<std::string_view>(S)),
              &ConstructInfiniteFeature::func)
    {
        this->document_synopsis("Returns a maximum / infinite duration.");
    }

    static R func() { return R::max(); }
};

} // namespace

namespace probfd {

static void register_fast_downward_types(plugins::Registry& registry)
{
    // Duration types
    plugins::Namespace& n = registry.get_global_name_space();

    n.insert_category_plugin<downward::utils::FNanoSeconds>(
        "nanoseconds",
        "Type representing a nanosecond.");
    n.insert_category_plugin<downward::utils::FMicroSeconds>(
        "microseconds",
        "Type representing a microsecond.");
    n.insert_category_plugin<downward::utils::FMilliSeconds>(
        "milliseconds",
        "Type representing a millisecond.");
    n.insert_category_plugin<downward::utils::FSeconds>(
        "seconds",
        "Type representing a second.");
    n.insert_category_plugin<downward::utils::FMinutes>(
        "minutes",
        "Type representing a minute.");
    n.insert_category_plugin<downward::utils::FHours>(
        "hours",
        "Type representing an hour.");

    // Cartesian abstractions
    cartesian_abstractions::add_subtask_generator_category(registry);

    // Evaluators
    evaluators::add_evaluator_category(registry);
    evaluators::add_evaluator_subcategory(registry);

    // Heuristics
    heuristics::add_additive_cartesian_heuristic_categories(registry);
    heuristics::add_landmark_cost_partitioning_heuristic_categories(registry);

    // Landmarks
    landmarks::add_landmark_factory_category(registry);

    // LP
    lp::add_lp_solver_enum(registry);

    // Merge-and-shrink
    merge_and_shrink::add_label_reduction_category(registry);
    merge_and_shrink::add_merge_scoring_function_category(registry);
    merge_and_shrink::add_merge_strategy_factory_category(registry);
    merge_and_shrink::add_merge_selector_category(registry);
    merge_and_shrink::add_merge_tree_factory_category(registry);
    merge_and_shrink::add_shrink_strategy_category(registry);

    // Mutexes
    mutexes::add_mutex_factory_category(registry);

    // Open Lists
    open_lists::add_open_list_factory_category(registry);

    // Operator Counting
    operator_counting::add_constraint_generator_category(registry);

    // PDBs
    pdbs::add_pattern_collection_generator_category(registry);
    pdbs::add_pdb_heuristic_subcategory(registry);
    pdbs::add_pattern_generator_category(registry);

    // Potentials
    potentials::add_potential_heuristics_subcategory(registry);

    // Pruning
    pruning::add_pruning_method_category(registry);

    // Search Algorithms
    search_algorithms::add_search_algorithm_factory_category(registry);

    // Task Transformations
    tasks::add_task_transformation_category(registry);

    // Utils
    utils::add_verbosity_enum(registry);

    // Operator Cost Enum
    add_operator_cost_category(registry);
}

static void register_fast_downward_definitions(plugins::Registry& registry)
{
    using namespace downward::utils::string_literals;

    plugins::Namespace& n = registry.get_global_name_space();

    // Infinity
    n.insert_feature_plugin<InfinityFeature>();

    // Generic literal suffixes
    n.insert_feature_plugin<
        LiteralFeature<int, 1'000, "__operator_int_k__"_sl>>();
    n.insert_feature_plugin<
        LiteralFeature<int, 1'000'000, "__operator_int_m__"_sl>>();
    n.insert_feature_plugin<
        LiteralFeature<int, 1'000'000'000, "__operator_int_g__"_sl>>();

    n.insert_feature_plugin<
        LiteralFeature<double, 1'000., "__operator_float_k__"_sl>>();
    n.insert_feature_plugin<
        LiteralFeature<double, 1'000'000., "__operator_float_m__"_sl>>();
    n.insert_feature_plugin<
        LiteralFeature<double, 1'000'000'000., "__operator_float_g__"_sl>>();

    // Duration literals
    n.insert_feature_plugin<CastFromLiteralFeature<
        downward::utils::FNanoSeconds,
        int,
        "__operator_int_ns__"_sl>>();
    n.insert_feature_plugin<CastFromLiteralFeature<
        downward::utils::FMicroSeconds,
        int,
        "__operator_int_us__"_sl>>();
    n.insert_feature_plugin<CastFromLiteralFeature<
        downward::utils::FMilliSeconds,
        int,
        "__operator_int_ms__"_sl>>();
    n.insert_feature_plugin<CastFromLiteralFeature<
        downward::utils::FSeconds,
        int,
        "__operator_int_s__"_sl>>();
    n.insert_feature_plugin<CastFromLiteralFeature<
        downward::utils::FMinutes,
        int,
        "__operator_int_min__"_sl>>();
    n.insert_feature_plugin<CastFromLiteralFeature<
        downward::utils::FHours,
        int,
        "__operator_int_h__"_sl>>();

    n.insert_feature_plugin<CastFromLiteralFeature<
        downward::utils::FNanoSeconds,
        double,
        "__operator_float_ns__"_sl>>();
    n.insert_feature_plugin<CastFromLiteralFeature<
        downward::utils::FMicroSeconds,
        double,
        "__operator_float_us__"_sl>>();
    n.insert_feature_plugin<CastFromLiteralFeature<
        downward::utils::FMilliSeconds,
        double,
        "__operator_float_ms__"_sl>>();
    n.insert_feature_plugin<CastFromLiteralFeature<
        downward::utils::FSeconds,
        double,
        "__operator_float_s__"_sl>>();
    n.insert_feature_plugin<CastFromLiteralFeature<
        downward::utils::FMinutes,
        double,
        "__operator_float_min__"_sl>>();
    n.insert_feature_plugin<CastFromLiteralFeature<
        downward::utils::FHours,
        double,
        "__operator_float_h__"_sl>>();

    // Infinite durations
    n.insert_feature_plugin<ConstructInfiniteFeature<
        downward::utils::FNanoSeconds,
        "nanoseconds_max"_sl>>();
    n.insert_feature_plugin<ConstructInfiniteFeature<
        downward::utils::FMicroSeconds,
        "microseconds_max"_sl>>();
    n.insert_feature_plugin<ConstructInfiniteFeature<
        downward::utils::FMilliSeconds,
        "milliseconds_max"_sl>>();
    n.insert_feature_plugin<ConstructInfiniteFeature<
        downward::utils::FSeconds,
        "seconds_max"_sl>>();
    n.insert_feature_plugin<ConstructInfiniteFeature<
        downward::utils::FMinutes,
        "minutes_max"_sl>>();
    n.insert_feature_plugin<
        ConstructInfiniteFeature<downward::utils::FHours, "hours_max"_sl>>();

    // Cartesian abstractions
    cartesian_abstractions::add_subtask_generators_features(registry);

    // Evaluators
    evaluators::add_const_evaluator_feature(registry);
    evaluators::add_g_evaluator_feature(registry);
    evaluators::add_max_evaluator_feature(registry);
    evaluators::add_pref_evaluator_feature(registry);
    evaluators::add_sum_evaluator_feature(registry);
    evaluators::add_weighted_evaluator_feature(registry);

    // Heuristics
    heuristics::add_additive_cartesian_heuristic_feature(registry);
    heuristics::add_additive_heuristic_feature(registry);
    heuristics::add_blind_heuristic_feature(registry);
    heuristics::add_canonical_pdbs_heuristic_feature(registry);
    heuristics::add_cg_heuristic_feature(registry);
    heuristics::add_cea_heuristic_feature(registry);
    heuristics::add_diverse_potential_heuristics_feature(registry);
    heuristics::add_ff_heuristic_features(registry);
    heuristics::add_goal_count_heuristic_features(registry);
    heuristics::add_hm_heuristic_features(registry);
    heuristics::add_pdb_heuristic_feature(registry);
    heuristics::add_ipdbs_heuristic_features(registry);
    heuristics::add_landmark_cost_partitioning_heuristic_feature(registry);
    heuristics::add_landmark_cut_heuristic_feature(registry);
    heuristics::add_max_heuristic_feature(registry);
    heuristics::add_landmark_sum_heuristic_feature(registry);
    heuristics::add_merge_and_shrink_heuristic_feature(registry);
    heuristics::add_operator_counting_heuristic_feature(registry);
    heuristics::add_sample_based_potential_heuristics_feature(registry);
    heuristics::add_single_potential_heuristics_features(registry);
    heuristics::add_zero_one_pdbs_heuristic_features(registry);

    // Landmarks
    landmarks::add_landmark_factory_hm_feature(registry);
    landmarks::add_landmark_factory_merged_feature(registry);
    landmarks::add_landmark_factory_reasonable_orders_hps_feature(registry);
    landmarks::add_landmark_factory_rpg_exhaust_feature(registry);
    landmarks::add_landmark_factory_rpg_sasp_feature(registry);
    landmarks::add_landmark_factory_zhu_givan_feature(registry);

    // Merge-and-shrink
    merge_and_shrink::add_label_reduction_features(registry);
    merge_and_shrink::add_merge_scoring_function_dfp_feature(registry);
    merge_and_shrink::add_merge_scoring_function_goal_relevance_feature(
        registry);
    merge_and_shrink::add_merge_scoring_function_miasm_feature(registry);
    merge_and_shrink::add_merge_scoring_function_total_order_feature(registry);
    merge_and_shrink::add_merge_scoring_function_single_random_feature(
        registry);
    merge_and_shrink::add_merge_selector_score_based_filtering_feature(
        registry);
    merge_and_shrink::add_merge_strategy_factory_precomputed_feature(registry);
    merge_and_shrink::add_merge_strategy_factory_sccs_feature(registry);
    merge_and_shrink::add_merge_strategy_factory_stateless_feature(registry);
    merge_and_shrink::add_merge_tree_factory_linear_feature(registry);
    merge_and_shrink::add_shrink_bisimulation_feature(registry);
    merge_and_shrink::add_shrink_fh_feature(registry);
    merge_and_shrink::add_shrink_random_feature(registry);

    // Mutexes
    mutexes::add_from_file_mutex_factory_feature(registry);

    // Open Lists
    open_lists::add_alternation_open_list_features(registry);
    open_lists::add_best_first_open_list_features(registry);
    open_lists::add_epsilon_greedy_open_list_features(registry);
    open_lists::add_pareto_open_list_features(registry);
    open_lists::add_tiebreaking_open_list_features(registry);
    open_lists::add_type_based_open_list_features(registry);

    // Operator Counting
    operator_counting::add_delete_relaxation_if_constraints_feature(registry);
    operator_counting::add_delete_relaxation_rr_constraints_feature(registry);
    operator_counting::add_lm_cut_constraints_feature(registry);
    operator_counting::add_pho_constraints_feature(registry);
    operator_counting::add_state_equation_constraints_feature(registry);

    // PDBs
    pdbs::add_pattern_collection_generator_combo_feature(registry);
    pdbs::add_pattern_collection_generator_disjoint_cegar_feature(registry);
    pdbs::add_pattern_collection_generator_genetic_feature(registry);
    pdbs::add_pattern_collection_generator_hillclimbing_feature(registry);
    pdbs::add_pattern_collection_generator_manual_feature(registry);
    pdbs::add_pattern_collection_generator_multiple_cegar_feature(registry);
    pdbs::add_pattern_collection_generator_multiple_random_feature(registry);
    pdbs::add_pattern_collection_generator_systematic_feature(registry);
    pdbs::add_pattern_generator_cegar_feature(registry);
    pdbs::add_pattern_generator_greedy_feature(registry);
    pdbs::add_pattern_generator_manual_feature(registry);
    pdbs::add_pattern_generator_random_feature(registry);

    // Pruning
    pruning::add_limited_pruning_feature(registry);
    pruning::add_null_pruning_method_feature(registry);
    pruning::add_stubborn_sets_atom_centric_feature(registry);
    pruning::add_stubborn_sets_ec_feature(registry);
    pruning::add_stubborn_sets_simple_feature(registry);

    // Search Algorithms
    search_algorithms::add_astar_feature(registry);
    search_algorithms::add_eager_feature(registry);
    search_algorithms::add_eager_greedy_feature(registry);
    search_algorithms::add_eager_wastar_feature(registry);
    search_algorithms::add_enforce_hill_climbing_search_feature(registry);
    search_algorithms::add_iterated_search_feature(registry);
    search_algorithms::add_lazy_feature(registry);
    search_algorithms::add_lazy_greedy_feature(registry);
    search_algorithms::add_lazy_wastar_feature(registry);

    // Task Transformations
    tasks::add_cost_task_transformation_features(registry);
    tasks::add_identity_task_transformation_features(registry);
}

static void register_probfd_types(plugins::Registry& registry)
{
    // Cartesian Abstractions
    probfd::cli::cartesian_abstractions::add_flaw_generator_category(registry);
    probfd::cli::cartesian_abstractions::add_subtask_generator_category(
        registry);
    probfd::cli::cartesian_abstractions::add_split_selector_category(registry);

    // Heuristics
    probfd::cli::heuristics::add_task_heuristic_factory_category(registry);

    // Merge-and-shrink
    probfd::cli::merge_and_shrink::add_label_reduction_category(registry);
    probfd::cli::merge_and_shrink::add_merge_scoring_function_category(
        registry);
    probfd::cli::merge_and_shrink::add_merge_selector_category(registry);
    probfd::cli::merge_and_shrink::add_merge_strategy_factory_category(
        registry);
    probfd::cli::merge_and_shrink::add_merge_tree_factory_category(registry);
    probfd::cli::merge_and_shrink::add_prune_strategy_category(registry);
    probfd::cli::merge_and_shrink::add_shrink_strategy_category(registry);

    // Occupation Measures
    probfd::cli::occupation_measures::add_constraint_generator_factory_category(
        registry);

    // Open Lists
    probfd::cli::open_lists::add_open_list_categories(registry);

    // PDBs
    probfd::cli::pdbs::add_pattern_collection_generator_category(registry);
    probfd::cli::pdbs::add_pattern_generator_category(registry);
    probfd::cli::pdbs::add_subcollection_finder_factory_category(registry);
    probfd::cli::pdbs::cegar::add_flaw_finding_strategy_category(registry);

    // Policy Pickers
    probfd::cli::policy_pickers::add_policy_picker_category(registry);

    // Solvers
    probfd::cli::solvers::add_task_solver_factory_category(registry);
    probfd::cli::solvers::add_statistical_mdp_algorithm_factory_category(
        registry);

    // Successor Samplers
    probfd::cli::successor_samplers::add_successor_sampler_category(registry);

    // Transition Sorters
    probfd::cli::transiton_sorters::add_transition_sorter_category(registry);

    // Task State Spaces
    probfd::cli::add_task_state_space_factory_category(registry);
}

static void register_probfd_definitions(plugins::Registry& registry)
{
    // Cartesian Abstractions
    probfd::cli::cartesian_abstractions::add_adaptive_flaw_generator_feature(
        registry);
    probfd::cli::cartesian_abstractions::add_astar_flaw_generator_feature(
        registry);
    probfd::cli::cartesian_abstractions::
        add_policy_based_flaw_generator_feature(registry);
    probfd::cli::cartesian_abstractions::add_split_selector_features(registry);
    probfd::cli::cartesian_abstractions::add_subtask_generator_features(
        registry);

    // Heuristics
    probfd::cli::heuristics::add_additive_cartesian_heuristic_feature(registry);
    probfd::cli::heuristics::add_dead_end_pruning_heuristic_feature(registry);
    probfd::cli::heuristics::add_gzocp_heuristic_feature(registry);
    probfd::cli::heuristics::add_merge_and_shrink_heuristic_feature(registry);
    probfd::cli::heuristics::add_blind_heuristic_factory_feature(registry);
    probfd::cli::heuristics::add_pdb_heuristic_feature(registry);
    probfd::cli::heuristics::add_scp_heuristic_feature(registry);
    probfd::cli::heuristics::add_ucp_heuristic_feature(registry);
    probfd::cli::heuristics::add_determinization_cost_heuristic_feature(
        registry);

    // Merge-and-shrink
    probfd::cli::merge_and_shrink::add_label_reduction_features(registry);
    probfd::cli::merge_and_shrink::add_merge_scoring_function_dfp_feature(
        registry);
    probfd::cli::merge_and_shrink::
        add_merge_scoring_function_goal_relevance_feature(registry);
    probfd::cli::merge_and_shrink::add_merge_scoring_function_miasm_feature(
        registry);
    probfd::cli::merge_and_shrink::
        add_merge_scoring_function_single_random_feature(registry);
    probfd::cli::merge_and_shrink::
        add_merge_scoring_function_total_order_feature(registry);
    probfd::cli::merge_and_shrink::
        add_merge_selector_score_based_filtering_feature(registry);
    probfd::cli::merge_and_shrink::
        add_merge_strategy_factory_precomputed_feature(registry);
    probfd::cli::merge_and_shrink::add_merge_strategy_factory_sccs_feature(
        registry);
    probfd::cli::merge_and_shrink::add_merge_strategy_factory_stateless_feature(
        registry);
    probfd::cli::merge_and_shrink::add_merge_tree_factory_linear_feature(
        registry);
    probfd::cli::merge_and_shrink::add_prune_strategy_alive_feature(registry);
    probfd::cli::merge_and_shrink::add_prune_strategy_identity_feature(
        registry);
    probfd::cli::merge_and_shrink::add_prune_strategy_solvable_feature(
        registry);
    probfd::cli::merge_and_shrink::add_shrink_strategy_bisimulation_feature(
        registry);
    probfd::cli::merge_and_shrink::add_shrink_strategy_equal_distance_feature(
        registry);
    probfd::cli::merge_and_shrink::add_shrink_strategy_random_feature(registry);
    probfd::cli::merge_and_shrink::
        add_shrink_strategy_probabilistic_bisimulation_feature(registry);

    // Occupation Measures
    probfd::cli::occupation_measures::
        add_occupation_measure_heuristics_features(registry);

    // Open Lists
    probfd::cli::open_lists::add_open_list_features(registry);

    // PDBs
    probfd::cli::pdbs::add_fully_additive_finder_factory_feature(registry);
    probfd::cli::pdbs::add_max_orthogonal_finder_factory_feature(registry);
    probfd::cli::pdbs::add_pattern_collection_generator_classical_feature(
        registry);
    probfd::cli::pdbs::add_pattern_collection_generator_disjoint_cegar_feature(
        registry);
    probfd::cli::pdbs::add_pattern_collection_generator_hillclimbing_feature(
        registry);
    probfd::cli::pdbs::add_pattern_collection_generator_multiple_cegar_feature(
        registry);
    probfd::cli::pdbs::add_pattern_collection_generator_systematic_feature(
        registry);
    probfd::cli::pdbs::add_trivial_finder_factory_feature(registry);
    probfd::cli::pdbs::cegar::add_bfs_flaw_finder_feature(registry);
    probfd::cli::pdbs::cegar::add_pucs_flaw_finder_feature(registry);
    probfd::cli::pdbs::cegar::add_sampling_flaw_finder_feature(registry);

    // Policy Pickers
    probfd::cli::policy_pickers::add_policy_picker_features(registry);

    // Solvers
    probfd::cli::solvers::add_acyclic_value_iteration_feature(registry);
    probfd::cli::solvers::add_aostar_solver_features(registry);
    probfd::cli::solvers::add_bisimulation_value_iteration_features(registry);
    probfd::cli::solvers::add_depth_first_heuristic_search_features(registry);
    probfd::cli::solvers::add_exhaustive_ao_solver_features(registry);
    probfd::cli::solvers::add_exhaustive_dfs_feature(registry);
    probfd::cli::solvers::add_i2dual_feature(registry);
    probfd::cli::solvers::add_idual_feature(registry);
    probfd::cli::solvers::add_interval_iteration_solver_feature(registry);
    probfd::cli::solvers::add_lrtdp_features(registry);
    probfd::cli::solvers::add_ta_depth_first_heuristic_search_feature(registry);
    probfd::cli::solvers::add_ta_topological_value_iteration_feature(registry);
    probfd::cli::solvers::add_ta_lrtdp_feature(registry);
    probfd::cli::solvers::add_topological_value_iteration_feature(registry);

    // Successor Samplers
    probfd::cli::successor_samplers::add_successor_sampler_features(registry);

    // Transition Sorters
    probfd::cli::transiton_sorters::add_transition_sorter_features(registry);

    // Task State Spaces
    probfd::cli::add_task_state_space_factory_features(registry);
}

void register_definitions(plugins::Registry& registry)
{
    register_fast_downward_types(registry);
    register_probfd_types(registry);

    register_fast_downward_definitions(registry);
    register_probfd_definitions(registry);
}

} // namespace probfd
