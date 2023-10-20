create_fast_downward_library(
    NAME core_sources
    HELP "Core source files"
    SOURCES
        downward/abstract_task
        downward/axioms
        downward/command_line
        downward/evaluation_context
        downward/evaluation_result
        downward/evaluator
        downward/evaluator_cache
        downward/heuristic
        downward/open_list
        downward/open_list_factory
        downward/operator_cost
        downward/operator_id
        downward/per_state_array
        downward/per_state_bitset
        downward/per_state_information
        downward/per_task_information
        downward/plan_manager
        downward/pruning_method
        downward/search_algorithm
        downward/search_node_info
        downward/search_progress
        downward/search_space
        downward/search_statistics
        downward/state_id
        downward/state_registry
        downward/task_id
        downward/task_proxy
    DEPENDS causal_graph int_hash_set int_packer ordered_set segmented_vector subscriber successor_generator task_properties
    CORE_LIBRARY
)

create_fast_downward_library(
    NAME plugins
    HELP "Plugin definition"
    SOURCES
        downward/plugins/any
        downward/plugins/bounds
        downward/plugins/doc_printer
        downward/plugins/options
        downward/plugins/plugin
        downward/plugins/plugin_info
        downward/plugins/raw_registry
        downward/plugins/registry
        downward/plugins/registry_types
        downward/plugins/types
    CORE_LIBRARY
)

create_fast_downward_library(
    NAME parser
    HELP "Option parsing"
    SOURCES
        downward/parser/abstract_syntax_tree
        downward/parser/decorated_abstract_syntax_tree
        downward/parser/lexical_analyzer
        downward/parser/syntax_analyzer
        downward/parser/token_stream
    CORE_LIBRARY
)

create_fast_downward_library(
    NAME utils
    HELP "System utilities"
    SOURCES
        downward/utils/collections
        downward/utils/countdown_timer
        downward/utils/exceptions
        downward/utils/hash
        downward/utils/logging
        downward/utils/markup
        downward/utils/math
        downward/utils/memory
        downward/utils/rng
        downward/utils/rng_options
        downward/utils/strings
        downward/utils/system
        downward/utils/system_unix
        downward/utils/system_windows
        downward/utils/timer
    CORE_LIBRARY
)

# On Linux, find the rt library for clock_gettime().
if(UNIX AND NOT APPLE)
    target_link_libraries(utils INTERFACE rt)
endif()

# On Windows, find the psapi library for determining peak memory.
if(WIN32)
    cmake_policy(SET CMP0074 NEW)
    target_link_libraries(utils INTERFACE psapi)
endif()

create_fast_downward_library(
    NAME alternation_open_list
    HELP "Open list that alternates between underlying open lists in a round-robin manner"
    SOURCES
        downward/open_lists/alternation_open_list
)

create_fast_downward_library(
    NAME best_first_open_list
    HELP "Open list that selects the best element according to a single evaluation function"
    SOURCES
        downward/open_lists/best_first_open_list
)

create_fast_downward_library(
    NAME epsilon_greedy_open_list
    HELP "Open list that chooses an entry randomly with probability epsilon"
    SOURCES
        downward/open_lists/epsilon_greedy_open_list
)

create_fast_downward_library(
    NAME pareto_open_list
    HELP "Pareto open list"
    SOURCES
        downward/open_lists/pareto_open_list
)

create_fast_downward_library(
    NAME tiebreaking_open_list
    HELP "Tiebreaking open list"
    SOURCES
        downward/open_lists/tiebreaking_open_list
)

create_fast_downward_library(
    NAME type_based_open_list
    HELP "Type-based open list"
    SOURCES
        downward/open_lists/type_based_open_list
)

create_fast_downward_library(
    NAME dynamic_bitset
    HELP "Poor man's version of boost::dynamic_bitset"
    SOURCES
        downward/algorithms/dynamic_bitset
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME named_vector
    HELP "Generic vector with associated name for each element"
    SOURCES
        downward/algorithms/named_vector
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME equivalence_relation
    HELP "Equivalence relation over [1, ..., n] that can be iteratively refined"
    SOURCES
        downward/algorithms/equivalence_relation
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME int_hash_set
    HELP "Hash set storing non-negative integers"
    SOURCES
        downward/algorithms/int_hash_set
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME int_packer
    HELP "Greedy bin packing algorithm to pack integer variables with small domains tightly into memory"
    SOURCES
        downward/algorithms/int_packer
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME max_cliques
    HELP "Implementation of the Max Cliques algorithm by Tomita et al."
    SOURCES
        downward/algorithms/max_cliques
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME priority_queues
    HELP "Three implementations of priority queue: HeapQueue, BucketQueue and AdaptiveQueue"
    SOURCES
        downward/algorithms/priority_queues
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME ordered_set
    HELP "Set of elements ordered by insertion time"
    SOURCES
        downward/algorithms/ordered_set
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME segmented_vector
    HELP "Memory-friendly and vector-like data structure"
    SOURCES
        downward/algorithms/segmented_vector
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME subscriber
    HELP "Allows object to subscribe to the destructor of other objects"
    SOURCES
        downward/algorithms/subscriber
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME evaluators_subcategory
    HELP "Subcategory plugin for basic evaluators"
    SOURCES
        downward/evaluators/subcategory
)

create_fast_downward_library(
    NAME const_evaluator
    HELP "The constant evaluator"
    SOURCES
        downward/evaluators/const_evaluator
    DEPENDS evaluators_subcategory
)

create_fast_downward_library(
    NAME g_evaluator
    HELP "The g-evaluator"
    SOURCES
        downward/evaluators/g_evaluator
    DEPENDS evaluators_subcategory
)

create_fast_downward_library(
    NAME combining_evaluator
    HELP "The combining evaluator"
    SOURCES
        downward/evaluators/combining_evaluator
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME max_evaluator
    HELP "The max evaluator"
    SOURCES
        downward/evaluators/max_evaluator
    DEPENDS combining_evaluator evaluators_subcategory
)

create_fast_downward_library(
    NAME pref_evaluator
    HELP "The pref evaluator"
    SOURCES
        downward/evaluators/pref_evaluator
    DEPENDS evaluators_subcategory
)

create_fast_downward_library(
    NAME weighted_evaluator
    HELP "The weighted evaluator"
    SOURCES
        downward/evaluators/weighted_evaluator
    DEPENDS evaluators_subcategory
)

create_fast_downward_library(
    NAME sum_evaluator
    HELP "The sum evaluator"
    SOURCES
        downward/evaluators/sum_evaluator
    DEPENDS combining_evaluator evaluators_subcategory
)

create_fast_downward_library(
    NAME null_pruning_method
    HELP "Pruning method that does nothing"
    SOURCES
        downward/pruning/null_pruning_method
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME limited_pruning
    HELP "Method for limiting another pruning method"
    SOURCES
        downward/pruning/limited_pruning
)

create_fast_downward_library(
    NAME stubborn_sets
    HELP "Base class for all stubborn set partial order reduction methods"
    SOURCES
        downward/pruning/stubborn_sets
    DEPENDS task_properties
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME stubborn_sets_action_centric
    HELP "Base class for all action-centric stubborn set partial order reduction methods"
    SOURCES
        downward/pruning/stubborn_sets_action_centric
    DEPENDS stubborn_sets
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME stubborn_sets_atom_centric
    HELP "Atom-centric stubborn sets"
    SOURCES
        downward/pruning/stubborn_sets_atom_centric
    DEPENDS stubborn_sets
)

create_fast_downward_library(
    NAME stubborn_sets_simple
    HELP "Stubborn sets simple"
    SOURCES
        downward/pruning/stubborn_sets_simple
    DEPENDS stubborn_sets_action_centric
)

create_fast_downward_library(
    NAME stubborn_sets_ec
    HELP "Stubborn set method that dominates expansion core"
    SOURCES
        downward/pruning/stubborn_sets_ec
    DEPENDS stubborn_sets_action_centric task_properties
)

create_fast_downward_library(
    NAME search_common
    HELP "Basic classes used for all search engines"
    SOURCES
        downward/search_algorithms/search_common
    DEPENDS alternation_open_list g_evaluator best_first_open_list sum_evaluator tiebreaking_open_list weighted_evaluator
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME eager_search
    HELP "Eager search algorithm"
    SOURCES
        downward/search_algorithms/eager_search
    DEPENDS null_pruning_method ordered_set successor_generator
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME plugin_astar
    HELP "A* search"
    SOURCES
        downward/search_algorithms/plugin_astar
    DEPENDS eager_search search_common
)

create_fast_downward_library(
    NAME plugin_eager
    HELP "Eager (i.e., normal) best-first search"
    SOURCES
        downward/search_algorithms/plugin_eager
    DEPENDS eager_search search_common
)

create_fast_downward_library(
    NAME plugin_eager_greedy
    HELP "Eager greedy best-first search"
    SOURCES
        downward/search_algorithms/plugin_eager_greedy
    DEPENDS eager_search search_common
)

create_fast_downward_library(
    NAME plugin_eager_wastar
    HELP "Weighted eager A* search"
    SOURCES
        downward/search_algorithms/plugin_eager_wastar
    DEPENDS eager_search search_common
)

create_fast_downward_library(
    NAME plugin_lazy
    HELP "Best-first search with deferred evaluation (lazy)"
    SOURCES
        downward/search_algorithms/plugin_lazy
    DEPENDS lazy_search search_common
)

create_fast_downward_library(
    NAME plugin_lazy_greedy
    HELP "Greedy best-first search with deferred evaluation (lazy)"
    SOURCES
        downward/search_algorithms/plugin_lazy_greedy
    DEPENDS lazy_search search_common
)

create_fast_downward_library(
    NAME plugin_lazy_wastar
    HELP "Weighted A* search with deferred evaluation (lazy)"
    SOURCES
        downward/search_algorithms/plugin_lazy_wastar
    DEPENDS lazy_search search_common
)

create_fast_downward_library(
    NAME enforced_hill_climbing_search
    HELP "Lazy enforced hill-climbing search algorithm"
    SOURCES
        downward/search_algorithms/enforced_hill_climbing_search
    DEPENDS g_evaluator ordered_set pref_evaluator search_common successor_generator
)

create_fast_downward_library(
    NAME iterated_search
    HELP "Iterated search algorithm"
    SOURCES
        downward/search_algorithms/iterated_search
)

create_fast_downward_library(
    NAME lazy_search
    HELP "Lazy search algorithm"
    SOURCES
        downward/search_algorithms/lazy_search
    DEPENDS ordered_set successor_generator
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME lp_solver
    HELP "Interface to an LP solver"
    SOURCES
        downward/lp/lp_solver
        downward/lp/solver_interface
    DEPENDS named_vector
    DEPENDENCY_ONLY
)

if(USE_LP)
    find_package(Cplex 12)
    if(CPLEX_FOUND)
        target_compile_definitions(lp_solver PUBLIC HAS_CPLEX)
        target_link_libraries(lp_solver PUBLIC cplex::cplex)
        target_sources(
            lp_solver
            PRIVATE
            ${PROJECT_SOURCE_DIR}/include/search/downward/lp/cplex_solver_interface.h
            ${PROJECT_SOURCE_DIR}/src/search/downward/lp/cplex_solver_interface.cc)
    endif()

    # TODO: we actually require a version greater than 6.0.3 but it is not released yet.
    find_package(soplex 6.0.3 QUIET)
    if (SOPLEX_FOUND)
        message(STATUS "Found SoPlex: ${SOPLEX_INCLUDE_DIRS}")
        target_compile_definitions(lp_solver PUBLIC HAS_SOPLEX)
        target_link_libraries(lp_solver PUBLIC libsoplex)
        target_sources(
            lp_solver
            PRIVATE
            ${PROJECT_SOURCE_DIR}/include/search/downward/lp/soplex_solver_interface.h
            ${PROJECT_SOURCE_DIR}/src/search/downward/lp/soplex_solver_interface.cc)
    endif()
endif()

create_fast_downward_library(
    NAME relaxation_heuristic
    HELP "The base class for relaxation heuristics"
    SOURCES
        downward/heuristics/array_pool
        downward/heuristics/relaxation_heuristic
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME additive_heuristic
    HELP "The additive heuristic"
    SOURCES
        downward/heuristics/additive_heuristic
    DEPENDS priority_queues relaxation_heuristic task_properties
)

create_fast_downward_library(
    NAME blind_search_heuristic
    HELP "The 'blind search' heuristic"
    SOURCES
        downward/heuristics/blind_search_heuristic
    DEPENDS task_properties
)

create_fast_downward_library(
    NAME context_enhanced_additive_heuristic
    HELP "The context-enhanced additive heuristic"
    SOURCES
        downward/heuristics/cea_heuristic
    DEPENDS domain_transition_graph priority_queues task_properties
)

create_fast_downward_library(
    NAME cg_heuristic
    HELP "The causal graph heuristic"
    SOURCES
        downward/heuristics/cg_heuristic
        downward/heuristics/cg_cache
    DEPENDS domain_transition_graph priority_queues task_properties
)

create_fast_downward_library(
    NAME domain_transition_graph
    HELP "DTGs used by cg and cea heuristic"
    SOURCES
        downward/heuristics/domain_transition_graph
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME ff_heuristic
    HELP "The FF heuristic (an implementation of the RPG heuristic)"
    SOURCES
        downward/heuristics/ff_heuristic
    DEPENDS additive_heuristic task_properties
)

create_fast_downward_library(
    NAME goal_count_heuristic
    HELP "The goal-counting heuristic"
    SOURCES
        downward/heuristics/goal_count_heuristic
)

create_fast_downward_library(
    NAME hm_heuristic
    HELP "The h^m heuristic"
    SOURCES
        downward/heuristics/hm_heuristic
    DEPENDS task_properties
)

create_fast_downward_library(
    NAME landmark_cut_heuristic
    HELP "The LM-cut heuristic"
    SOURCES
        downward/heuristics/lm_cut_heuristic
        downward/heuristics/lm_cut_landmarks
    DEPENDS priority_queues task_properties
)

create_fast_downward_library(
    NAME max_heuristic
    HELP "The Max heuristic"
    SOURCES
        downward/heuristics/max_heuristic
    DEPENDS priority_queues relaxation_heuristic
)

create_fast_downward_library(
    NAME core_tasks
    HELP "Core task transformations"
    SOURCES
        downward/tasks/cost_adapted_task
        downward/tasks/delegating_task
        downward/tasks/root_task
    CORE_LIBRARY
)

create_fast_downward_library(
    NAME extra_tasks
    HELP "Non-core task transformations"
    SOURCES
        downward/tasks/domain_abstracted_task
        downward/tasks/domain_abstracted_task_factory
        downward/tasks/modified_goals_task
        downward/tasks/modified_operator_costs_task
    DEPENDS task_properties
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME causal_graph
    HELP "Causal Graph"
    SOURCES
        downward/task_utils/causal_graph
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME sampling
    HELP "Sampling"
    SOURCES
        downward/task_utils/sampling
    DEPENDS successor_generator task_properties
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME successor_generator
    HELP "Successor generator"
    SOURCES
        downward/task_utils/successor_generator
        downward/task_utils/successor_generator_factory
        downward/task_utils/successor_generator_internals
    DEPENDS task_properties
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME task_properties
    HELP "Task properties"
    SOURCES
        downward/task_utils/task_properties
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME variable_order_finder
    HELP "Variable order finder"
    SOURCES
        downward/task_utils/variable_order_finder
    DEPENDENCY_ONLY
)

create_fast_downward_library(
    NAME cartesian_abstractions
    HELP "Plugin containing the code for Cartesian CEGAR heuristics"
    SOURCES
        downward/cartesian_abstractions/abstraction
        downward/cartesian_abstractions/abstract_search
        downward/cartesian_abstractions/abstract_state
        downward/cartesian_abstractions/additive_cartesian_heuristic
        downward/cartesian_abstractions/cartesian_heuristic_function
        downward/cartesian_abstractions/cartesian_set
        downward/cartesian_abstractions/cegar
        downward/cartesian_abstractions/cost_saturation
        downward/cartesian_abstractions/refinement_hierarchy
        downward/cartesian_abstractions/split_selector
        downward/cartesian_abstractions/subtask_generators
        downward/cartesian_abstractions/transition
        downward/cartesian_abstractions/transition_system
        downward/cartesian_abstractions/types
        downward/cartesian_abstractions/utils
        downward/cartesian_abstractions/utils_landmarks
    DEPENDS additive_heuristic dynamic_bitset extra_tasks landmarks priority_queues task_properties
)

create_fast_downward_library(
    NAME mas_heuristic
    HELP "The Merge-and-Shrink heuristic"
    SOURCES
        downward/merge_and_shrink/distances
        downward/merge_and_shrink/factored_transition_system
        downward/merge_and_shrink/fts_factory
        downward/merge_and_shrink/label_reduction
        downward/merge_and_shrink/labels
        downward/merge_and_shrink/merge_and_shrink_algorithm
        downward/merge_and_shrink/merge_and_shrink_heuristic
        downward/merge_and_shrink/merge_and_shrink_representation
        downward/merge_and_shrink/merge_scoring_function
        downward/merge_and_shrink/merge_scoring_function_dfp
        downward/merge_and_shrink/merge_scoring_function_goal_relevance
        downward/merge_and_shrink/merge_scoring_function_miasm
        downward/merge_and_shrink/merge_scoring_function_miasm_utils
        downward/merge_and_shrink/merge_scoring_function_single_random
        downward/merge_and_shrink/merge_scoring_function_total_order
        downward/merge_and_shrink/merge_selector
        downward/merge_and_shrink/merge_selector_score_based_filtering
        downward/merge_and_shrink/merge_strategy
        downward/merge_and_shrink/merge_strategy_factory
        downward/merge_and_shrink/merge_strategy_factory_precomputed
        downward/merge_and_shrink/merge_strategy_factory_sccs
        downward/merge_and_shrink/merge_strategy_factory_stateless
        downward/merge_and_shrink/merge_strategy_precomputed
        downward/merge_and_shrink/merge_strategy_sccs
        downward/merge_and_shrink/merge_strategy_stateless
        downward/merge_and_shrink/merge_tree
        downward/merge_and_shrink/merge_tree_factory
        downward/merge_and_shrink/merge_tree_factory_linear
        downward/merge_and_shrink/shrink_bisimulation
        downward/merge_and_shrink/shrink_bucket_based
        downward/merge_and_shrink/shrink_fh
        downward/merge_and_shrink/shrink_random
        downward/merge_and_shrink/shrink_strategy
        downward/merge_and_shrink/transition_system
        downward/merge_and_shrink/types
        downward/merge_and_shrink/utils
    DEPENDS priority_queues equivalence_relation sccs task_properties variable_order_finder
)

create_fast_downward_library(
    NAME landmarks
    HELP "Plugin containing the code to reason with landmarks"
    SOURCES
        downward/landmarks/exploration
        downward/landmarks/landmark
        downward/landmarks/landmark_cost_partitioning_algorithms
        downward/landmarks/landmark_cost_partitioning_heuristic
        downward/landmarks/landmark_factory
        downward/landmarks/landmark_factory_h_m
        downward/landmarks/landmark_factory_reasonable_orders_hps
        downward/landmarks/landmark_factory_merged
        downward/landmarks/landmark_factory_relaxation
        downward/landmarks/landmark_factory_rpg_exhaust
        downward/landmarks/landmark_factory_rpg_sasp
        downward/landmarks/landmark_factory_zhu_givan
        downward/landmarks/landmark_graph
        downward/landmarks/landmark_heuristic
        downward/landmarks/landmark_status_manager
        downward/landmarks/landmark_sum_heuristic
        downward/landmarks/util
    DEPENDS lp_solver priority_queues successor_generator task_properties
)

create_fast_downward_library(
    NAME operator_counting
    HELP "Plugin containing the code for operator-counting heuristics"
    SOURCES
        downward/operator_counting/constraint_generator
        downward/operator_counting/delete_relaxation_constraints
        downward/operator_counting/lm_cut_constraints
        downward/operator_counting/operator_counting_heuristic
        downward/operator_counting/pho_constraints
        downward/operator_counting/state_equation_constraints
    DEPENDS lp_solver landmark_cut_heuristic pdbs task_properties
)

create_fast_downward_library(
    NAME pdbs
    HELP "Plugin containing the code for PDBs"
    SOURCES
        downward/pdbs/canonical_pdbs
        downward/pdbs/canonical_pdbs_heuristic
        downward/pdbs/cegar
        downward/pdbs/dominance_pruning
        downward/pdbs/incremental_canonical_pdbs
        downward/pdbs/match_tree
        downward/pdbs/max_cliques
        downward/pdbs/pattern_cliques
        downward/pdbs/pattern_collection_information
        downward/pdbs/pattern_collection_generator_combo
        downward/pdbs/pattern_collection_generator_disjoint_cegar
        downward/pdbs/pattern_collection_generator_genetic
        downward/pdbs/pattern_collection_generator_hillclimbing
        downward/pdbs/pattern_collection_generator_manual
        downward/pdbs/pattern_collection_generator_multiple_cegar
        downward/pdbs/pattern_collection_generator_multiple_random
        downward/pdbs/pattern_collection_generator_multiple
        downward/pdbs/pattern_collection_generator_systematic
        downward/pdbs/pattern_database
        downward/pdbs/pattern_generator_cegar
        downward/pdbs/pattern_generator_greedy
        downward/pdbs/pattern_generator_manual
        downward/pdbs/pattern_generator_random
        downward/pdbs/pattern_generator
        downward/pdbs/pattern_information
        downward/pdbs/pdb_heuristic
        downward/pdbs/random_pattern
        downward/pdbs/subcategory
        downward/pdbs/types
        downward/pdbs/utils
        downward/pdbs/validation
        downward/pdbs/zero_one_pdbs
        downward/pdbs/zero_one_pdbs_heuristic
    DEPENDS causal_graph max_cliques priority_queues sampling successor_generator task_properties variable_order_finder
)

create_fast_downward_library(
    NAME potentials
    HELP "Plugin containing the code for potential heuristics"
    SOURCES
        downward/potentials/diverse_potential_heuristics
        downward/potentials/potential_function
        downward/potentials/potential_heuristic
        downward/potentials/potential_max_heuristic
        downward/potentials/potential_optimizer
        downward/potentials/sample_based_potential_heuristics
        downward/potentials/single_potential_heuristics
        downward/potentials/subcategory
        downward/potentials/util
    DEPENDS lp_solver sampling successor_generator task_properties
)

create_fast_downward_library(
    NAME sccs
    HELP "Algorithm to compute the strongly connected components (SCCs) of a "
         "directed graph."
    SOURCES
        downward/algorithms/sccs
    DEPENDENCY_ONLY
)