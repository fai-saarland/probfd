create_library(
    NAME plugins
    HELP "Plugin definition"
    SOURCES
        downward/cli/plugins/any
        downward/cli/plugins/bounds
        downward/cli/plugins/doc_printer
        downward/cli/plugins/options
        downward/cli/plugins/plugin
        downward/cli/plugins/plugin_info
        downward/cli/plugins/raw_registry
        downward/cli/plugins/registry
        downward/cli/plugins/registry_types
        downward/cli/plugins/types
)

create_library(
    NAME parser
    HELP "Option parsing"
    SOURCES
        downward/cli/parser/abstract_syntax_tree
        downward/cli/parser/decorated_abstract_syntax_tree
        downward/cli/parser/lexical_analyzer
        downward/cli/parser/syntax_analyzer
        downward/cli/parser/token_stream
)

create_library(
    NAME core_categories
    HELP "Core categories"
    SOURCES
        downward/cli/task_transformation_category
    DEPENDS
        plugins
        core_sources
    TARGET
        probfd
)

create_library(
    NAME evaluator_category
    HELP "Evaluator plugin category"
    SOURCES
        downward/cli/evaluator_category
    DEPENDS
        plugins
        core_sources
    TARGET
        probfd
)

create_library(
    NAME evaluator_options
    HELP "Evaluator base options"
    SOURCES
        downward/cli/evaluator_options
    DEPENDS
        plugins
        logging_options
        utils
)

create_library(
    NAME heuristic_options
    HELP "Heuristic base options"
    SOURCES
        downward/cli/heuristic_options
    DEPENDS
        plugins
        evaluator_options
        utils
)

create_library(
    NAME open_list_factory_category
    HELP "Open list factory plugin category"
    SOURCES
        downward/cli/open_list_factory_category
    DEPENDS
        plugins
        core_sources
    TARGET
        probfd
)

create_library(
    NAME open_list_options
    HELP "Open list base options"
    SOURCES
        downward/cli/open_list_options
    DEPENDS
        plugins
)

create_library(
    NAME operator_cost_options
    HELP "Operator cost options"
    SOURCES
        downward/cli/operator_cost_options
    DEPENDS
        plugins
        core_sources
)

create_library(
    NAME pruning_method_category
    HELP "Pruning method category"
    SOURCES
        downward/cli/pruning_method_category
    DEPENDS
        plugins
        core_sources
    TARGET
        probfd
)

create_library(
    NAME pruning_method_options
    HELP "Pruning method base options"
    SOURCES
        downward/cli/pruning_method_options
    DEPENDS
        plugins
        utils
        logging_options
)

create_library(
    NAME search_algorithm_category
    HELP "Search algorithm category"
    SOURCES
        downward/cli/search_algorithm_category
    DEPENDS
        plugins
        core_sources
    TARGET
        probfd
)

create_library(
    NAME search_algorithm_factory_category
    HELP "Search algorithm factory category"
    SOURCES
        downward/cli/search_algorithm_factory_category
    DEPENDS
        plugins
        core_sources
    TARGET
        probfd
)

create_library(
    NAME search_algorithm_options
    HELP "Search algorithm base options"
    SOURCES
        downward/cli/search_algorithm_options
    DEPENDS
        plugins
        operator_cost_options
        logging_options
        rng_options
        core_sources
        utils
)

create_library(
    NAME alternation_open_list_feature
    HELP "Open list that alternates between underlying open lists in a round-robin manner"
    SOURCES
        downward/cli/open_lists/alternation_open_list_feature
    DEPENDS
        plugins
        alternation_open_list
    TARGET
        probfd
)

create_library(
    NAME best_first_open_list_feature
    HELP "Open list that selects the best element according to a single evaluation function"
    SOURCES
        downward/cli/open_lists/best_first_open_list_feature
    DEPENDS
        plugins
        open_list_options
        best_first_open_list
        utils
    TARGET
        probfd
)

create_library(
    NAME epsilon_greedy_open_list_feature
    HELP "Open list that chooses an entry randomly with probability epsilon"
    SOURCES
        downward/cli/open_lists/epsilon_greedy_open_list_feature
    DEPENDS
        plugins
        open_list_options
        rng_options
        epsilon_greedy_open_list
        utils
    TARGET
        probfd
)

create_library(
    NAME pareto_open_list_feature
    HELP "Pareto open list"
    SOURCES
        downward/cli/open_lists/pareto_open_list_feature
    DEPENDS
        plugins
        open_list_options
        rng_options
        pareto_open_list
        utils
    TARGET
        probfd
)

create_library(
    NAME tiebreaking_open_list_feature
    HELP "Tiebreaking open list"
    SOURCES
        downward/cli/open_lists/tiebreaking_open_list_feature
    DEPENDS
        plugins
        open_list_options
        tiebreaking_open_list
        utils
    TARGET
        probfd
)

create_library(
    NAME type_based_open_list_feature
    HELP "Type-based open list"
    SOURCES
        downward/cli/open_lists/type_based_open_list_feature
    DEPENDS
        plugins
        open_list_options
        rng_options
        type_based_open_list
        utils
    TARGET
        probfd
)

create_library(
    NAME evaluators_subcategory
    HELP "Subcategory plugin for basic evaluators"
    SOURCES
        downward/cli/evaluators/subcategory
    DEPENDS
        plugins
    TARGET
        probfd
)

create_library(
    NAME combining_evaluator_options
    HELP "The combining evaluator options"
    SOURCES
        downward/cli/evaluators/combining_evaluator_options
    DEPENDS
        plugins
        evaluator_options
        utils
        core_sources
)

create_library(
    NAME const_evaluator_feature
    HELP "The constant evaluator"
    SOURCES
        downward/cli/evaluators/const_evaluator_feature
    DEPENDS
        plugins
        evaluator_options
        const_evaluator
        utils
    TARGET
        probfd
)

create_library(
    NAME g_evaluator_feature
    HELP "The g-evaluator"
    SOURCES
        downward/cli/evaluators/g_evaluator_feature
    DEPENDS
        plugins
        evaluator_options
        g_evaluator
        utils
    TARGET
        probfd
)

create_library(
    NAME max_evaluator_feature
    HELP "The max evaluator"
    SOURCES
        downward/cli/evaluators/max_evaluator_feature
    DEPENDS
        plugins
        combining_evaluator_options
        max_evaluator
        utils
    TARGET
        probfd
)

create_library(
    NAME pref_evaluator_feature
    HELP "The pref evaluator"
    SOURCES
        downward/cli/evaluators/pref_evaluator_feature
    DEPENDS
        plugins
        evaluator_options
        pref_evaluator
        utils
    TARGET
        probfd
)

create_library(
    NAME sum_evaluator_feature
    HELP "The sum evaluator"
    SOURCES
        downward/cli/evaluators/sum_evaluator_feature
    DEPENDS
        plugins
        combining_evaluator_options
        sum_evaluator
        utils
    TARGET
        probfd
)

create_library(
    NAME weighted_evaluator_feature
    HELP "The weighted evaluator"
    SOURCES
        downward/cli/evaluators/weighted_evaluator_feature
    DEPENDS
        plugins
        evaluator_options
        weighted_evaluator
        utils
    TARGET
        probfd
)

create_library(
    NAME null_pruning_method_feature
    HELP "Pruning method that does nothing"
    SOURCES
        downward/cli/pruning/null_pruning_method_feature
    DEPENDS
        plugins
        pruning_method_options
        null_pruning_method
        utils
    TARGET
        probfd
)

create_library(
    NAME limited_pruning_feature
    HELP "Method for limiting another pruning method"
    SOURCES
        downward/cli/pruning/limited_pruning_feature
    DEPENDS
        plugins
        pruning_method_options
        limited_pruning
        utils
    TARGET
        probfd
)

create_library(
    NAME stubborn_sets_atom_centric_feature
    HELP "Atom-centric stubborn sets"
    SOURCES
        downward/cli/pruning/stubborn_sets_atom_centric_feature
    DEPENDS
        plugins
        pruning_method_options
        stubborn_sets_atom_centric
        utils
    TARGET
        probfd
)

create_library(
    NAME stubborn_sets_simple_feature
    HELP "Stubborn sets simple"
    SOURCES
        downward/cli/pruning/stubborn_sets_simple_feature
    DEPENDS
        plugins
        pruning_method_options
        stubborn_sets_simple
        utils
    TARGET
        probfd
)

create_library(
    NAME stubborn_sets_ec_feature
    HELP "Stubborn set method that dominates expansion core"
    SOURCES
        downward/cli/pruning/stubborn_sets_ec_feature
    DEPENDS
        plugins
        pruning_method_options
        stubborn_sets_ec
        utils
    TARGET
        probfd
)

create_library(
    NAME eager_search_options
    HELP "Eager search options"
    SOURCES
        downward/cli/search_algorithms/eager_search_options
    DEPENDS
        plugins
        search_algorithm_options
        open_list_options
        utils
)

create_library(
    NAME astar_feature
    HELP "A* search feature"
    SOURCES
        downward/cli/search_algorithms/astar_feature
    DEPENDS
        plugins
        eager_search_options
        eager_search
        search_common
    TARGET
        probfd
)

create_library(
    NAME eager_feature
    HELP "Eager (i.e., normal) best-first search"
    SOURCES
        downward/cli/search_algorithms/eager_feature
    DEPENDS
        plugins
        eager_search_options
        eager_search
        search_common
    TARGET
        probfd
)

create_library(
    NAME eager_greedy_feature
    HELP "Eager greedy best-first search"
    SOURCES
        downward/cli/search_algorithms/eager_greedy_feature
    DEPENDS
        plugins
        eager_search_options
        eager_search
        search_common
    TARGET
        probfd
)

create_library(
    NAME plugin_eager_wastar
    HELP "Weighted eager A* search"
    SOURCES
        downward/cli/search_algorithms/eager_wastar_feature
    DEPENDS
        plugins
        eager_search_options
        eager_search
        search_common
    TARGET
        probfd
)

create_library(
    NAME plugin_lazy
    HELP "Best-first search with deferred evaluation (lazy)"
    SOURCES
        downward/cli/search_algorithms/lazy_feature
    DEPENDS
        plugins
        search_algorithm_options
        lazy_search
        search_common
    TARGET
        probfd
)

create_library(
    NAME plugin_lazy_greedy
    HELP "Greedy best-first search with deferred evaluation (lazy)"
    SOURCES
        downward/cli/search_algorithms/lazy_greedy_feature
    DEPENDS
        plugins
        search_algorithm_options
        lazy_search
        search_common
    TARGET
        probfd
)

create_library(
    NAME plugin_lazy_wastar
    HELP "Weighted A* search with deferred evaluation (lazy)"
    SOURCES
        downward/cli/search_algorithms/lazy_wastar_feature
    DEPENDS
        plugins
        search_algorithm_options
        lazy_search
        search_common
    TARGET
        probfd
)

create_library(
    NAME enforced_hill_climbing_search_feature
    HELP "Lazy enforced hill-climbing search algorithm"
    SOURCES
        downward/cli/search_algorithms/enforced_hill_climbing_search_feature
    DEPENDS
        plugins
        search_algorithm_options
        enforced_hill_climbing_search
        utils
    TARGET
        probfd
)

create_library(
    NAME iterated_search_feature
    HELP "Iterated search algorithm"
    SOURCES
        downward/cli/search_algorithms/iterated_search_feature
    DEPENDS
        plugins
        search_algorithm_options
        iterated_search
        utils
    TARGET
        probfd
)

create_library(
    NAME lp_solver_options
    HELP "LP solver options"
    SOURCES
        downward/cli/lp/lp_solver_options
    DEPENDS
        plugins
        lp_solver
)

create_library(
    NAME additive_heuristic_feature
    HELP "The additive heuristic"
    SOURCES
        downward/cli/heuristics/additive_heuristic_feature
    DEPENDS
        plugins
        heuristic_options
        additive_heuristic
    TARGET
        probfd
)

create_library(
    NAME blind_search_heuristic_feature
    HELP "The 'blind search' heuristic"
    SOURCES
        downward/cli/heuristics/blind_search_heuristic_feature
    DEPENDS
        plugins
        heuristic_options
        blind_search_heuristic
        utils
    TARGET
        probfd
)

create_library(
    NAME context_enhanced_additive_heuristic_feature
    HELP "The context-enhanced additive heuristic"
    SOURCES
        downward/cli/heuristics/cea_heuristic_feature
    DEPENDS
        plugins
        heuristic_options
        context_enhanced_additive_heuristic
        utils
    TARGET
        probfd
)

create_library(
    NAME cg_heuristic_feature
    HELP "The causal graph heuristic"
    SOURCES
        downward/cli/heuristics/cg_heuristic_feature
    DEPENDS
        plugins
        heuristic_options
        cg_heuristic
        utils
    TARGET
        probfd
)

create_library(
    NAME ff_heuristic_feature
    HELP "The FF heuristic (an implementation of the RPG heuristic)"
    SOURCES
        downward/cli/heuristics/ff_heuristic_feature
    DEPENDS
        plugins
        heuristic_options
        ff_heuristic
        utils
    TARGET
        probfd
)

create_library(
    NAME goal_count_heuristic_feature
    HELP "The goal-counting heuristic"
    SOURCES
        downward/cli/heuristics/goal_count_heuristic_feature
    DEPENDS
        plugins
        heuristic_options
        goal_count_heuristic
        utils
    TARGET
        probfd
)

create_library(
    NAME hm_heuristic_feature
    HELP "The h^m heuristic"
    SOURCES
        downward/cli/heuristics/hm_heuristic_feature
    DEPENDS
        plugins
        heuristic_options
        hm_heuristic
        utils
    TARGET
        probfd
)

create_library(
    NAME landmark_cut_heuristic_feature
    HELP "The LM-cut heuristic"
    SOURCES
        downward/cli/heuristics/lm_cut_heuristic_feature
    DEPENDS
        plugins
        heuristic_options
        landmark_cut_heuristic
        utils
    TARGET
        probfd
)

create_library(
    NAME max_heuristic_feature
    HELP "The Max heuristic"
    SOURCES
        downward/cli/heuristics/max_heuristic_feature
    DEPENDS
        plugins
        heuristic_options
        max_heuristic
        utils
    TARGET
        probfd
)

create_library(
    NAME cost_task_transformation_feature
    HELP "Cost-adapted task transformation feature"
    SOURCES
        downward/cli/tasks/cost_task_transformation_feature
    DEPENDS
        plugins
        operator_cost_options
        core_sources
        core_tasks
    TARGET
        probfd
)

create_library(
    NAME identity_task_transformation_feature
    HELP "Identity task transformation feature"
    SOURCES
        downward/cli/tasks/identity_task_transformation_feature
    DEPENDS
        plugins
        core_sources
    TARGET
        probfd
)

create_library(
    NAME constraint_generator_category
    HELP "Constraint generator category"
    SOURCES
        downward/cli/operator_counting/constraint_generator_category
    DEPENDS
        plugins
        operator_counting
    TARGET
        probfd
)

create_library(
    NAME delete_relaxation_if_constraints_feature
    HELP "Delete relaxtion if constraints feature"
    SOURCES
        downward/cli/operator_counting/delete_relaxation_if_constraints_feature
    DEPENDS
        plugins
        operator_counting
        lp_solver
        utils
    TARGET
        probfd
)

create_library(
    NAME delete_relaxation_rr_constraints_feature
    HELP "Delete relaxtion rr constraints feature"
    SOURCES
        downward/cli/operator_counting/delete_relaxation_rr_constraints_feature
    DEPENDS
        plugins
        operator_counting
        lp_solver
        utils
    TARGET
        probfd
)

create_library(
    NAME lm_cut_constraints_feature
    HELP "LM-Cut constraints feature"
    SOURCES
        downward/cli/operator_counting/lm_cut_constraints_feature
    DEPENDS
        plugins
        operator_counting
        utils
    TARGET
        probfd
)

create_library(
    NAME operator_counting_heuristic_feature
    HELP "Operator-counting heuristic feature"
    SOURCES
        downward/cli/operator_counting/operator_counting_heuristic_feature
    DEPENDS
        plugins
        heuristic_options
        lp_solver_options
        operator_counting
        utils
    TARGET
        probfd
)

create_library(
    NAME pho_constraints_feature
    HELP "Post-hoc optimization constraints feature"
    SOURCES
        downward/cli/operator_counting/pho_constraints_feature
    DEPENDS
        plugins
        operator_counting
        utils
    TARGET
        probfd
)

create_library(
    NAME state_equation_constraints_feature
    HELP "State-equation constraints feature"
    SOURCES
        downward/cli/operator_counting/state_equation_constraints_feature
    DEPENDS
        plugins
        logging_options
        operator_counting
        utils
    TARGET
        probfd
)

create_library(
    NAME logging_options
    HELP "Logging options"
    SOURCES
        downward/cli/utils/logging_options
    DEPENDS
        plugins
        utils
)

create_library(
    NAME rng_options
    HELP "RNG options"
    SOURCES
        downward/cli/utils/rng_options
    DEPENDS
        plugins
)

create_library(
    NAME potentials_subcategory
    HELP "Potentials sub-category"
    SOURCES
        downward/cli/potentials/subcategory
    DEPENDS
        plugins
    TARGET
        probfd
)

create_library(
    NAME potential_options
    HELP "Potentials options"
    SOURCES
        downward/cli/potentials/potential_options
    DEPENDS
        plugins
        heuristic_options
        lp_solver_options
        utils
)

create_library(
    NAME diverse_potential_heuristics_feature
    HELP "Sample-based potential heuristics feature"
    SOURCES
        downward/cli/potentials/diverse_potential_heuristics_feature
    DEPENDS
        plugins
        potential_options
        rng_options
        potentials
        utils
    TARGET
        probfd
)

create_library(
    NAME sample_based_potential_heuristics_feature
    HELP "Sample-based potential heuristics feature"
    SOURCES
        downward/cli/potentials/sample_based_potential_heuristics_feature
    DEPENDS
        plugins
        potential_options
        rng_options
        potentials
        utils
    TARGET
        probfd
)

create_library(
    NAME single_potential_heuristics_feature
    HELP "Single potential heuristics feature"
    SOURCES
        downward/cli/potentials/single_potential_heuristics_feature
    DEPENDS
        plugins
        potential_options
        potentials
        utils
    TARGET
        probfd
)

create_library(
    NAME subtask_generators_category
    HELP "Sub-task generators category"
    SOURCES
        downward/cli/cartesian_abstractions/subtask_generators_category
    DEPENDS
        plugins
        cartesian_abstractions
    TARGET
        probfd
)

create_library(
    NAME subtask_generators_features
    HELP "Sub-task generators features"
    SOURCES
        downward/cli/cartesian_abstractions/subtask_generators_feature
    DEPENDS
        plugins
        rng_options
        cartesian_abstractions
        additive_heuristic
        landmarks
        task_properties
        extra_tasks
        utils
    TARGET
        probfd
)

create_library(
    NAME additive_cartesian_heuristic_feature
    HELP "Additive cartesian heuristic feature"
    SOURCES
        downward/cli/cartesian_abstractions/additive_cartesian_heuristic_feature
    DEPENDS
        plugins
        heuristic_options
        rng_options
        cartesian_abstractions
        utils
    TARGET
        probfd
)

create_library(
    NAME landmark_heuristic_options
    HELP "Landmark heuristic options"
    SOURCES
        downward/cli/landmarks/landmark_heuristic_options
    DEPENDS
        plugins
        heuristic_options
        landmarks
        utils
)

create_library(
    NAME landmark_cost_partitioning_heuristic_feature
    HELP "Landmark cost-partitioning heuristic feature"
    SOURCES
        downward/cli/landmarks/landmark_cost_partitioning_heuristic_feature
    DEPENDS
        plugins
        landmark_heuristic_options
        lp_solver_options
        landmarks
        utils
    TARGET
        probfd
)

create_library(
    NAME landmark_sum_heuristic_feature
    HELP "Landmark sum heuristic feature"
    SOURCES
        downward/cli/landmarks/landmark_sum_heuristic_feature
    DEPENDS
        plugins
        landmark_heuristic_options
        landmarks
        utils
    TARGET
        probfd
)

create_library(
    NAME landmark_factory_options
    HELP "Landmark factory options"
    SOURCES
        downward/cli/landmarks/landmark_factory_options
    DEPENDS
        plugins
        logging_options
        landmarks
)

create_library(
    NAME landmark_factory_h_m_feature
    HELP "Landmark factory options"
    SOURCES
        downward/cli/landmarks/landmark_factory_h_m_feature
    DEPENDS
        plugins
        landmark_factory_options
        landmarks
        utils
    TARGET
        probfd
)

create_library(
    NAME landmark_factory_merged_feature
    HELP "Landmark factory options"
    SOURCES
        downward/cli/landmarks/landmark_factory_merged_feature
    DEPENDS
        plugins
        landmark_factory_options
        landmarks
        utils
    TARGET
        probfd
)

create_library(
    NAME landmark_factory_reasonable_orders_hps_feature
    HELP "Landmark factory options"
    SOURCES
        downward/cli/landmarks/landmark_factory_reasonable_orders_hps_feature
    DEPENDS
        plugins
        landmark_factory_options
        landmarks
        utils
    TARGET
        probfd
)

create_library(
    NAME landmark_factory_rpg_exhaust_feature
    HELP "Landmark factory options"
    SOURCES
        downward/cli/landmarks/landmark_factory_rpg_exhaust_feature
    DEPENDS
        plugins
        landmark_factory_options
        landmarks
    TARGET
        probfd
)

create_library(
    NAME landmark_factory_rpg_sasp_feature
    HELP "Landmark factory options"
    SOURCES
        downward/cli/landmarks/landmark_factory_rpg_sasp_feature
    DEPENDS
        plugins
        landmark_factory_options
        landmarks
    TARGET
        probfd
)

create_library(
    NAME landmark_factory_zhu_givan_feature
    HELP "Landmark factory options"
    SOURCES
        downward/cli/landmarks/landmark_factory_zhu_givan_feature
    DEPENDS
        plugins
        landmark_factory_options
        landmarks
    TARGET
        probfd
)

create_library(
    NAME label_reduction_features
    HELP "Label reduction features"
    SOURCES
        downward/cli/merge_and_shrink/label_reduction_feature
    DEPENDS
        plugins
        rng_options
        mas_heuristic
        utils
    TARGET
        probfd
)

create_library(
    NAME merge_and_shrink_algorithm_options
    HELP "Merge-and-shrink algorithm options"
    SOURCES
        downward/cli/merge_and_shrink/merge_and_shrink_algorithm_options
    DEPENDS
        plugins
        mas_heuristic
        utils
)

create_library(
    NAME merge_and_shrink_heuristic_feature
    HELP "Merge-and-shrink heuristic feature"
    SOURCES
        downward/cli/merge_and_shrink/merge_and_shrink_heuristic_feature
    DEPENDS
        plugins
        merge_and_shrink_algorithm_options
        heuristic_options
        mas_heuristic
        utils
    TARGET
        probfd
)

create_library(
    NAME merge_scoring_function_category
    HELP "Merge scoring function category"
    SOURCES
        downward/cli/merge_and_shrink/merge_scoring_function_category
    DEPENDS
        plugins
        mas_heuristic
    TARGET
        probfd
)

create_library(
    NAME merge_scoring_function_dfp_feature
    HELP "Merge scoring function DFP feature"
    SOURCES
        downward/cli/merge_and_shrink/merge_scoring_function_dfp_feature
    DEPENDS
        plugins
        mas_heuristic
        utils
    TARGET
        probfd
)

create_library(
    NAME merge_scoring_function_goal_relevance_feature
    HELP "Merge scoring function goal relevance feature"
    SOURCES
        downward/cli/merge_and_shrink/merge_scoring_function_goal_relevance_feature
    DEPENDS
        plugins
        mas_heuristic
    TARGET
        probfd
)

create_library(
    NAME merge_scoring_function_miasm_feature
    HELP "Merge scoring function miasm feature"
    SOURCES
        downward/cli/merge_and_shrink/merge_scoring_function_miasm_feature
    DEPENDS
        plugins
        merge_and_shrink_algorithm_options
        mas_heuristic
        utils
    TARGET
        probfd
)

create_library(
    NAME merge_scoring_function_single_random_feature
    HELP "Merge scoring function single random feature"
    SOURCES
        downward/cli/merge_and_shrink/merge_scoring_function_single_random_feature
    DEPENDS
        plugins
        rng_options
        mas_heuristic
        utils
    TARGET
        probfd
)

create_library(
    NAME merge_scoring_function_total_order_feature
    HELP "Merge scoring function total order feature"
    SOURCES
        downward/cli/merge_and_shrink/merge_scoring_function_total_order_feature
    DEPENDS
        plugins
        rng_options
        mas_heuristic
        utils
    TARGET
        probfd
)

create_library(
    NAME merge_selector_category
    HELP "Merge selector category"
    SOURCES
        downward/cli/merge_and_shrink/merge_selector_category
    DEPENDS
        plugins
        mas_heuristic
    TARGET
        probfd
)

create_library(
    NAME merge_selector_score_based_filtering_feature
    HELP "Merge selector score-based filtering feature"
    SOURCES
        downward/cli/merge_and_shrink/merge_selector_score_based_filtering_feature
    DEPENDS
        plugins
        mas_heuristic
    TARGET
        probfd
)

create_library(
    NAME merge_strategy_factory_category
    HELP "Merge strategy factory category"
    SOURCES
        downward/cli/merge_and_shrink/merge_strategy_factory_category
    DEPENDS
        plugins
        mas_heuristic
    TARGET
        probfd
)

create_library(
    NAME merge_strategy_factory_precomputed_feature
    HELP "Merge strategy factory precomputed feature"
    SOURCES
        downward/cli/merge_and_shrink/merge_strategy_factory_precomputed_feature
    DEPENDS
        plugins
        merge_strategy_options
        mas_heuristic
    TARGET
        probfd
)

create_library(
    NAME merge_strategy_factory_sccs_feature
    HELP "Merge strategy factory SCCs feature"
    SOURCES
        downward/cli/merge_and_shrink/merge_strategy_factory_sccs_feature
    DEPENDS
        plugins
        merge_strategy_options
        mas_heuristic
        utils
    TARGET
        probfd
)

create_library(
    NAME merge_strategy_factory_stateless_feature
    HELP "Merge strategy factory stateless feature"
    SOURCES
        downward/cli/merge_and_shrink/merge_strategy_factory_stateless_feature
    DEPENDS
        plugins
        merge_strategy_options
        mas_heuristic
    TARGET
        probfd
)

create_library(
    NAME merge_strategy_options
    HELP "Merge strategy options"
    SOURCES
        downward/cli/merge_and_shrink/merge_strategy_options
    DEPENDS
        plugins
        logging_options
    TARGET
        probfd
)

create_library(
    NAME merge_tree_factory_category
    HELP "Merge tree factory category"
    SOURCES
        downward/cli/merge_and_shrink/merge_tree_factory_category
    DEPENDS
        plugins
        mas_heuristic
    TARGET
        probfd
)

create_library(
    NAME merge_tree_factory_linear_feature
    HELP "Merge tree factory linear feature"
    SOURCES
        downward/cli/merge_and_shrink/merge_tree_factory_linear_feature
    DEPENDS
        plugins
        merge_tree_options
        mas_heuristic
        utils
    TARGET
        probfd
)

create_library(
    NAME merge_tree_options
    HELP "Merge tree options"
    SOURCES
        downward/cli/merge_and_shrink/merge_tree_options
    DEPENDS
        plugins
        rng_options
)

create_library(
    NAME shrink_bisimulation_feature
    HELP "Shrink bisimulation feature"
    SOURCES
        downward/cli/merge_and_shrink/shrink_bisimulation_feature
    DEPENDS
        plugins
        mas_heuristic
        utils
    TARGET
        probfd
)

create_library(
    NAME shrink_bucket_based_options
    HELP "Bucket-based shrink strategy options"
    SOURCES
        downward/cli/merge_and_shrink/shrink_bucket_based_options
    DEPENDS
        plugins
        rng_options
)

create_library(
    NAME shrink_fh_feature
    HELP "FH shrink strategy feature"
    SOURCES
        downward/cli/merge_and_shrink/shrink_fh_feature
    DEPENDS
        plugins
        shrink_bucket_based_options
        mas_heuristic
        utils
    TARGET
        probfd
)

create_library(
    NAME shrink_random_feature
    HELP "Random shrink strategy feature"
    SOURCES
        downward/cli/merge_and_shrink/shrink_random_feature
    DEPENDS
        plugins
        shrink_bucket_based_options
        mas_heuristic
    TARGET
        probfd
)

create_library(
    NAME shrink_strategy_category
    HELP "Shrink strategy category"
    SOURCES
        downward/cli/merge_and_shrink/shrink_strategy_category
    DEPENDS
        plugins
        mas_heuristic
    TARGET
        probfd
)

create_library(
    NAME canonical_pdbs_heuristic_feature
    HELP "Canoncial PDBs heuristic feature"
    SOURCES
        downward/cli/pdbs/canonical_pdbs_heuristic_feature
    DEPENDS
        plugins
        heuristic_options
        canonical_pdbs_heuristic_options
        pdbs
    TARGET
        probfd
)

create_library(
    NAME canonical_pdbs_heuristic_options
    HELP "Canoncial PDBs heuristic options"
    SOURCES
        downward/cli/pdbs/canonical_pdbs_heuristic_options
    DEPENDS
        plugins
        pdbs
)

create_library(
    NAME cegar_options
    HELP "CEGAR options"
    SOURCES
        downward/cli/pdbs/cegar_options
    DEPENDS
        plugins
)

create_library(
    NAME pattern_collection_generator_combo_feature
    HELP "Pattern collection generator combo feature"
    SOURCES
        downward/cli/pdbs/pattern_collection_generator_combo_feature
    DEPENDS
        plugins
        pattern_generator_options
        pdbs
    TARGET
        probfd
)

create_library(
    NAME pattern_collection_generator_disjoint_cegar_feature
    HELP "Pattern collection generator disjoint cegar feature"
    SOURCES
        downward/cli/pdbs/pattern_collection_generator_disjoint_cegar_feature
    DEPENDS
        plugins
        rng_options
        cegar_options
        pattern_generator_options
        pdb_utils
        pdbs
    TARGET
        probfd
)

create_library(
    NAME pattern_collection_generator_genetic_feature
    HELP "Pattern collection generator genetic feature"
    SOURCES
        downward/cli/pdbs/pattern_collection_generator_genetic_feature
    DEPENDS
        plugins
        rng_options
        pattern_generator_options
        pdbs
        utils
    TARGET
        probfd
)

create_library(
    NAME pattern_collection_generator_hillclimbing_feature
    HELP "Pattern collection generator hillclimbing feature"
    SOURCES
        downward/cli/pdbs/pattern_collection_generator_hillclimbing_feature
    DEPENDS
        plugins
        pattern_collection_generator_hillclimbing_options
        canonical_pdbs_heuristic_options
        pattern_generator_options
        heuristic_options
        pdbs
        utils
    TARGET
        probfd
)

create_library(
    NAME pattern_collection_generator_hillclimbing_options
    HELP "Pattern collection generator hillclimbing options"
    SOURCES
        downward/cli/pdbs/pattern_collection_generator_hillclimbing_options
    DEPENDS
        plugins
        rng_options
)

create_library(
    NAME pattern_collection_generator_manual_feature
    HELP "Pattern collection generator manual feature"
    SOURCES
        downward/cli/pdbs/pattern_collection_generator_manual_feature
    DEPENDS
        plugins
        pattern_generator_options
        pdbs
    TARGET
        probfd
)

create_library(
    NAME pattern_collection_generator_multiple_cegar_feature
    HELP "Pattern collection generator multiple CEGAR feature"
    SOURCES
        downward/cli/pdbs/pattern_collection_generator_multiple_cegar_feature
    DEPENDS
        plugins
        cegar_options
        pattern_collection_generator_multiple_options
        pdb_utils
        pdbs
        utils
    TARGET
        probfd
)

create_library(
    NAME pattern_collection_generator_multiple_options
    HELP "Pattern collection generator multiple options"
    SOURCES
        downward/cli/pdbs/pattern_collection_generator_multiple_options
    DEPENDS
        plugins
        pattern_collection_generator_multiple_options
        pattern_generator_options
        rng_options
        utils
)

create_library(
    NAME pattern_collection_generator_multiple_random_feature
    HELP "Pattern collection generator multiple random feature"
    SOURCES
        downward/cli/pdbs/pattern_collection_generator_multiple_random_feature
    DEPENDS
        plugins
        pattern_collection_generator_multiple_options
        random_pattern_options
        pdb_utils
        pdbs
    TARGET
        probfd
)

create_library(
    NAME pattern_collection_generator_systematic_feature
    HELP "Pattern collection generator systematic feature"
    SOURCES
        downward/cli/pdbs/pattern_collection_generator_systematic_feature
    DEPENDS
        plugins
        pattern_generator_options
        pdbs
        utils
    TARGET
        probfd
    TARGET
        probfd
)

create_library(
    NAME pattern_generator_category
    HELP "Pattern generator category"
    SOURCES
        downward/cli/pdbs/pattern_generator_category
    DEPENDS
        plugins
        pdbs
    TARGET
        probfd
)

create_library(
    NAME pattern_generator_cegar_feature
    HELP "Pattern generator CEGAR feature"
    SOURCES
        downward/cli/pdbs/pattern_generator_cegar_feature
    DEPENDS
        plugins
        cegar_options
        pattern_generator_options
        pdb_utils
        rng_options
        pdbs
        utils
    TARGET
        probfd
)

create_library(
    NAME pattern_generator_greedy_feature
    HELP "Pattern generator greedy feature"
    SOURCES
        downward/cli/pdbs/pattern_generator_greedy_feature
    DEPENDS
        plugins
        pattern_generator_options
        pdbs
    TARGET
        probfd
)

create_library(
    NAME pattern_generator_manual_feature
    HELP "Pattern generator manual feature"
    SOURCES
        downward/cli/pdbs/pattern_generator_manual_feature
    DEPENDS
        plugins
        pattern_generator_options
        pdbs
    TARGET
        probfd
)

create_library(
    NAME pattern_generator_options
    HELP "Pattern generator options"
    SOURCES
        downward/cli/pdbs/pattern_generator_options
    DEPENDS
        plugins
        logging_options
)

create_library(
    NAME pattern_generator_random_feature
    HELP "Random pattern generator feature"
    SOURCES
        downward/cli/pdbs/pattern_generator_random_feature
    DEPENDS
        plugins
        pattern_generator_options
        random_pattern_options
        pdb_utils
        rng_options
        pdbs
)

create_library(
    NAME pdb_heuristic_feature
    HELP "PDB heuristic feature"
    SOURCES
        downward/cli/pdbs/pdb_heuristic_feature
    DEPENDS
        plugins
        heuristic_options
        pdbs
    TARGET
        probfd
)

create_library(
    NAME random_pattern_options
    HELP "Random pattern generation options"
    SOURCES
        downward/cli/pdbs/random_pattern_options
    DEPENDS
        plugins
)

create_library(
    NAME pdb_subcategory
    HELP "Pattern databases sub-category"
    SOURCES
        downward/cli/pdbs/subcategory
    DEPENDS
        plugins
    TARGET
        probfd
)

create_library(
    NAME pdb_utils
    HELP "Pattern database utility functions"
    SOURCES
        downward/cli/pdbs/utils
    DEPENDS
        utils
    TARGET
        probfd
)

create_library(
    NAME zero_one_pdbs_heuristic_feature
    HELP "Zero-one PDB heuristic feature"
    SOURCES
        downward/cli/pdbs/zero_one_pdbs_heuristic_feature
    DEPENDS
        plugins
        heuristic_options
        pdbs
    TARGET
        probfd
)